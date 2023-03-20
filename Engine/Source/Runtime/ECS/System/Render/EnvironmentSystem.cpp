#include "flamepch.h"

#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/ECS/System/Render/EnvironmentSystem.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

#include "Runtime/Renderer/Renderer3D.h"

#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/UniformBufferLibrary.h"

#include "Runtime/Renderer/Framebuffer.h"

#include "Editor/Panels/SceneSettingsPanel.h"
#include <glad/glad.h>


namespace Flame
{
	static uint32_t id = 0;
	static uint32_t oldId = 0;

	void EnvironmentSystem::OnUpdateRuntime(Timestep ts)
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

		switch (ModeManager::m_SceneMode)
		{
		case SceneMode::None:
			break;
		case SceneMode::SkyBox:
			if (mainCamera)
				DrawSkyBox(glm::inverse(cameraTransform), mainCamera->GetProjection());
			break;
		default:
			break;
		}
	}

	void EnvironmentSystem::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		switch (ModeManager::m_SceneMode)
		{
			case SceneMode::None:
				break;
			case SceneMode::SkyBox:
				DrawSkyBox(camera.GetViewMatrix(), camera.GetProjection());
				break;
			default:
				break;
		}
	}

	void EnvironmentSystem::DrawSkyBox(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = ProjectMatrix * glm::mat4(glm::mat3(ViewMatrix)); // 这样平移不会使得天空盒在视角中移动
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));

		RenderCommand::Cull(0);

		RenderCommand::DepthFunc(DepthComp::LEQUAL); // 深度缓冲将会填充上天空盒的1.0值
		{
			Library<Shader>::GetInstance().GetSkyBoxShader()->Bind();
			Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture")->Bind(0);
			Library<Shader>::GetInstance().GetSkyBoxShader()->SetInt("SkyBox", 0);
			Library<Mesh>::GetInstance().Get("Box")->Draw();
		}
		RenderCommand::DepthFunc(DepthComp::LESS);
	}
}

