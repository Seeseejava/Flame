#include "flamepch.h"

#include "Runtime/ECS/System/Render/RenderSystem3D.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

#include "Runtime/Renderer/Renderer3D.h"

namespace Flame
{
	void RenderSystem3D::OnUpdateRuntime(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Scene->m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer3D::BeginScene(*mainCamera, cameraTransform);

			// Draw model
			{
				auto view = m_Scene->m_Registry.view<TransformComponent, StaticMeshComponent>();
				for (auto entity : view)
				{
					auto [transform, mesh] = view.get<TransformComponent, StaticMeshComponent>(entity);

					Renderer3D::DrawModel(transform.GetTransform(), mesh, (int)entity);
				}
			}

			Renderer3D::EndScene();
		}
	}

	void RenderSystem3D::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer3D::BeginScene(camera);

		auto group = m_Scene->m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent>);

		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, StaticMeshComponent>(entity);

			Renderer3D::DrawModel(transform.GetTransform(), mesh, (int)entity);
		}

		Renderer3D::EndScene();

		Renderer3D::DrawSkyBox(camera);
	}
}