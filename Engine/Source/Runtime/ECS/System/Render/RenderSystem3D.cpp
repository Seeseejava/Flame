#include "flamepch.h"

#include "Runtime/ECS/System/Render/RenderSystem3D.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

#include "Runtime/Renderer/Renderer3D.h"

namespace Flame
{
	void RenderSystem3D::OnUpdateRuntime(Timestep ts)
	{
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
	}
}