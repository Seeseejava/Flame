#include "flamepch.h"
#include "Scene.h"

#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Core/Core.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/System/SystemGroup.h"
#include "Runtime/Renderer/Renderer3D.h"

#include <glm/glm.hpp>

#include "Runtime/ECS/Entity/Entity.h"

// Box2D
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include <box2d/b2_circle_shape.h>

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

		if (ModeManager::b3DMode)
		{
			m_Systems.clear();
			m_Systems.emplace_back(std::make_unique<RenderSystem3D>(this));
			m_Systems.emplace_back(std::make_unique<PhysicsSystem3D>(this));
		}
		else
		{
			m_Systems.clear();
			m_Systems.emplace_back(std::make_unique<PhysicsSystem2D>(this));
			m_Systems.emplace_back(std::make_unique<NativeScriptSystem>(this));
			m_Systems.emplace_back(std::make_unique<RenderSystem2D>(this));
		}
	}

	Scene::~Scene()
	{

	}

	template<typename Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = src.view<Component>();
		for (auto e : view)
		{
			UUID uuid = src.get<IDComponent>(e).ID;
			FLAME_CORE_ASSERT(enttMap.find(uuid) != enttMap.end(), "No such uuid");
			entt::entity dstEnttID = enttMap.at(uuid);

			auto& component = src.get<Component>(e);
			dst.emplace_or_replace<Component>(dstEnttID, component);
		}
	}

	template<typename Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		if (src.HasComponent<Component>())
			dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = std::make_shared<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	void Scene::ChangeDimMode()
	{
		int nowDimMode = ModeManager::b3DMode;
		if (nowDimMode)
		{
			m_Systems.clear();
			m_Systems.emplace_back(std::make_unique<RenderSystem3D>(this));
			m_Systems.emplace_back(std::make_unique<PhysicsSystem3D>(this));
		}
		else
		{
			m_Systems.clear();
			m_Systems.emplace_back(std::make_unique<PhysicsSystem2D>(this));
			m_Systems.emplace_back(std::make_unique<NativeScriptSystem>(this));
			m_Systems.emplace_back(std::make_unique<RenderSystem2D>(this));
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };  //����m_EntityHandle = 0�ˣ���Ϊcreate����������0��
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		for (auto& system : m_Systems)
		{
			system->OnRuntiemStart();
		}
		
	}

	void Scene::OnRuntimeStop()
	{
		for (auto& system : m_Systems)
		{
			system->OnRuntimeStop();
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		for (auto& system : m_Systems)
		{
			system->OnUpdateRuntime(ts);
		}
		
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		for (auto& system : m_Systems)
		{
			system->OnUpdateEditor(ts, camera);
		}

		Renderer3D::BeginScene(camera);

		auto group = m_Registry.group<TransformComponent>(entt::get<StaticMeshComponent>);

		for (auto entity : group)
		{
			auto [transform, mesh] = group.get<TransformComponent, StaticMeshComponent>(entity);

			Renderer3D::DrawModel(transform.GetTransform(), mesh, (int)entity);
		}

		Renderer3D::EndScene();
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

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);

		CopyComponentIfExists<TransformComponent>(newEntity, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity, entity);
		CopyComponentIfExists<CameraComponent>(newEntity, entity);
		CopyComponentIfExists<Rigidbody2DComponent>(newEntity, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity, entity);
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	// ��Flame��д�˸�ģ���࣬Ȼ������EditorLayer��������ģ���࣬
	// ���������ڣ�Ŀǰ�������ڲ�û�е��õ�AddComponent���룬���¸�ģ��û�б����ɳ�������ʱEditorLayer����OnComponentAdded��ʧ�ܣ�������Ҫ�ֶ��ø�OnComponentAdded���Ա��롣
	// ������������²���Ҫ�õ�������ܣ���������£�ģ��Ӧ�ñ�������header�ļ���
	// ע��, ���ﲻ��ģ���ػ�, �����ñ����������⼸���ض����͵�ģ�庯������   ?

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<StaticMeshComponent>(Entity entity, StaticMeshComponent& component)
	{
		component.Mesh = Model(component.Path.string());
	}
}