#include "flamepch.h"
#include "Scene.h"

#include "Components.h"
#include "Flame/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace Flame {

	// ���ں����Callback����, ��Transform���������ʱ����, ��ӵ�entity��
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity) {}

	Scene::Scene()
	{
#if ENTT_EXAMPLE_CODE
		// ����һ��TransformComponent��
		struct TransformComponent
		{
			glm::mat4 Transform{ 1.0f };

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default; //?
			TransformComponent(const glm::mat4& transform)
				: Transform(transform) {}

			operator glm::mat4& () { return Transform; }
			operator const glm::mat4& () const { return Transform; } // ������ʽ����ת��
		};
		struct MeshComponent
		{
			MeshComponent() = default;
			float value;
		};

		// ����һ��registry, ���԰������Ϊvector<entity>, Ҳ���ǰ�������entity������
		entt::registry m_Registry;

		// ����һ��entity, entt::entity��ʵ��uint32_t
		entt::entity entity = m_Registry.create();
		// emplace��ͬ��AddComponent, �����entity���TransformComponent
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));// ����Ĳ����ᴫ��TransformComponent�Ĺ��캯��

		// entt�ṩ��Callback, ��TransformComponent������ʱ, ����OnTransformConstruct����
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		// �ж�entity���Ƿ���TransformComponent, �൱��HasComponent
		if (m_Registry.all_of<TransformComponent>(entity))
			// ��entity��get TransformComponent, �൱��GetComponent
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		// ��ȡ���д���TransformComponent��entity����
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		// group������ȡͬʱ����ӵ�ж��Component��Entity����, ����õ���group��
		// m_Registry�����м���TransformComponent������MeshComponent��Entity����
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		// ����д�в���?
		//auto group = m_Registry.group<TransformComponent, MeshComponent>();
		for (auto entity : group)
		{
			// transform��mesh���Ǽ�¼������
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity); // ֮ǰ����ʱ��ΪMeshComponent�ǿյ�
		}
#endif
	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };  //����m_EntityHandle = 0�ˣ���Ϊcreate����������0��
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = &transform.Transform;
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}

	}

}