#include "flamepch.h"

#include "Runtime/ECS/System/Render/RenderSystem3D.h"
#include "Runtime/ECS/System/Render/EnvironmentSystem.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/UniformBuffer.h"
#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace Flame
{
	namespace Utils
	{
		static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
		{
			const auto inv = glm::inverse(projview);

			std::vector<glm::vec4> frustumCorners;
			for (unsigned int x = 0; x < 2; ++x)
			{
				for (unsigned int y = 0; y < 2; ++y)
				{
					for (unsigned int z = 0; z < 2; ++z)
					{
						const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
						frustumCorners.push_back(pt / pt.w);
					}
				}
			}

			return frustumCorners;
		}

		static	glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, float cameraFOV, float aspect, const glm::vec3& lightDir, const glm::mat4& view)
		{
			const auto proj = glm::perspective(
				glm::radians(cameraFOV), aspect, nearPlane,
				farPlane);
			const auto corners = getFrustumCornersWorldSpace(proj * view);

			glm::vec3 center = glm::vec3(0, 0, 0);
			for (const auto& v : corners)
			{
				center += glm::vec3(v);
			}
			center /= corners.size();  // 视锥体中心

			const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();
			for (const auto& v : corners)
			{
				const auto trf = lightView * v;
				minX = std::min(minX, trf.x);
				maxX = std::max(maxX, trf.x);
				minY = std::min(minY, trf.y);
				maxY = std::max(maxY, trf.y);
				minZ = std::min(minZ, trf.z);
				maxZ = std::max(maxZ, trf.z);
			}

			// Tune this parameter according to the scene
			constexpr float zMult = 10.0f;
			if (minZ < 0)
			{
				minZ *= zMult;
			}
			else
			{
				minZ /= zMult;
			}
			if (maxZ < 0)
			{
				maxZ /= zMult;
			}
			else
			{
				maxZ *= zMult;
			}

			const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

			return lightProjection * lightView;
		}
	}

	void RenderSystem3D::OnUpdateRuntime(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		glm::vec3 cameraPos;
		{
			auto view = m_Scene->m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					cameraPos = transform.GetTranslation();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer3D::BeginScene(*mainCamera, cameraTransform);

			// Get the Light 
			{

			}

			// Draw model
			{
				auto view = m_Scene->m_Registry.view<TransformComponent, MeshComponent>();
				for (auto entity : view)
				{
					auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

					Renderer3D::DrawModel(transform.GetTransform(), cameraPos, mesh, (int)entity);
				}
			}

			Renderer3D::EndScene();
		}
	}

	void RenderSystem3D::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer3D::BeginScene(camera);

		// shader configuration
		Ref<Shader> defaultShader = Library<Shader>::GetInstance().GetDefaultShader();


		// Point Light
		{
			auto view = m_Scene->m_Registry.view<TransformComponent, PointLightComponent>();
			int i = 0;
			for (auto entity : view)
			{
				auto [transform, light] = view.get<TransformComponent, PointLightComponent>(entity);

				glm::vec3 lightPos = transform.GetTranslation();
				float intensity = light.Intensity;
				glm::vec3 lightColor = light.LightColor;

				defaultShader->Bind();
				defaultShader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPos);
				defaultShader->SetFloat3("lightColors[" + std::to_string(i) + "]", intensity * lightColor);

				i++;
			}
			if (i == 0)
			{
				for (size_t i = 0; i < 4; i++)
				{
					defaultShader->Bind();
					defaultShader->SetFloat3("lightColors[" + std::to_string(i) + "]", glm::vec3{ -1.0f });
				}
			}
		}

		// Directional light 
		{
			auto view = m_Scene->m_Registry.view<TransformComponent, DirectionalLightComponent>();

			defaultShader->Bind();
			defaultShader->SetInt("shadowMap", 8);

			for (auto e : view)
			{
				Entity entity = { e, m_Scene };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& directionalLight = entity.GetComponent<DirectionalLightComponent>();

				glm::vec3 lightDir = glm::normalize(glm::eulerAngles(glm::quat(transform.Rotation)));
				float cameraNearPlane = camera.GetNearPlane();
				float cameraFarPlane = camera.GetFarPlane();

				defaultShader->SetFloat3("lightDir", lightDir);
				defaultShader->SetFloat("dirLightIntensity", directionalLight.Intensity);
				defaultShader->SetFloat("farPlane", cameraFarPlane);

				const auto lightMatrix = Utils::getLightSpaceMatrix(cameraNearPlane, cameraFarPlane, camera.GetFOV(),
					camera.GetAspect(), lightDir, camera.GetViewMatrix());

				Ref<UniformBuffer> lightMatricesUBO = Library<UniformBuffer>::GetInstance().Get("LightMatrixUniform");

				lightMatricesUBO->SetData(&lightMatrix, sizeof(glm::mat4));

				break; // now we only support one directional light
			}
		}

		uint32_t mainFramebuffer = RenderCommand::GetDrawFrameBuffer();

		// Light Depth pass

		Renderer3D::lightFBO->Bind();
		Renderer3D::lightFBO->BindDepthTex2D(8);

		RenderCommand::SetViewport(0, 0, Renderer3D::lightFBO->GetSpecification().Width, Renderer3D::lightFBO->GetSpecification().Height);
		RenderCommand::Clear();

		RenderCommand::CullFrontOrBack(true); // fix peter panning
		auto view = m_Scene->m_Registry.view<TransformComponent, MeshComponent>();
		for (auto e : view)
		{
			Entity entity = { e, m_Scene };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& mesh = entity.GetComponent<MeshComponent>();

			Ref<Shader> smShader = Library<Shader>::GetInstance().Get("SM_Depth");
			smShader->Bind();
			if (mesh.m_Mesh->bPlayAnim)
				smShader->SetBool("u_Animated", true);
			else
				smShader->SetBool("u_Animated", false);

			mesh.m_Mesh->Draw(transform.GetTransform(), camera.GetPosition(), smShader, (int)e);
		}
		RenderCommand::CullFrontOrBack(false);

		// Render pass
		RenderCommand::BindFrameBuffer(mainFramebuffer);
		RenderCommand::SetViewport(0, 0, ConfigManager::m_ViewportSize.x, ConfigManager::m_ViewportSize.y);
		for (auto e : view)
		{
			Entity entity = { e, m_Scene };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& mesh = entity.GetComponent<MeshComponent>();
			if ((int)e == ConfigManager::selectedEntity)
			{
				RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 1, 0xFF);
				RenderCommand::StencilMask(0xFF);
				Renderer3D::DrawModel(transform.GetTransform(), camera.GetPosition(), mesh, (int)e);

				RenderCommand::SetStencilFunc(StencilFunc::NOTEQUAL, 1, 0xFF);
				RenderCommand::StencilMask(0x00);
				//RenderCommand::DepthTest(false);
				if (mesh.m_Mesh->bPlayAnim)
					mesh.m_Mesh->Draw(transform.GetTransform(), camera.GetPosition(), Library<Shader>::GetInstance().Get("NormalOutline_anim"), (int)e);
				else
					mesh.m_Mesh->Draw(transform.GetTransform(), camera.GetPosition(), Library<Shader>::GetInstance().Get("NormalOutline"), (int)e);
				RenderCommand::StencilMask(0xFF);
				RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 0, 0xFF);
				//RenderCommand::DepthTest(true);
				RenderCommand::ClearStencil();
			}
			else
				Renderer3D::DrawModel(transform.GetTransform(), camera.GetPosition(), mesh, (int)e);
		}
		Renderer3D::EndScene();

	}
}