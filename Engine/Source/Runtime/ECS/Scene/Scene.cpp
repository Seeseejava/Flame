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

	// 用于后面的Callback例子, 当Transform组件被创建时调用, 会加到entity上
	static void OnTransformConstruct(entt::registry& registry, entt::entity entity) {}

	
	Scene::Scene()
	{
#if ENTT_EXAMPLE_CODE
		// 创建一个TransformComponent类
		struct TransformComponent
		{
			glm::mat4 Transform{ 1.0f };

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default; //?
			TransformComponent(const glm::mat4& transform)
				: Transform(transform) {}

			operator glm::mat4& () { return Transform; }
			operator const glm::mat4& () const { return Transform; } // 重载隐式类型转换
		};
		struct MeshComponent
		{
			MeshComponent() = default;
			float value;
		};

		// 创建一个registry, 可以把它理解为vector<entity>, 也就是包含所有entity的容器
		entt::registry m_Registry;

		// 创建一个entity, entt::entity其实是uint32_t
		entt::entity entity = m_Registry.create();
		// emplace等同于AddComponent, 这里给entity添加TransformComponent
		m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));// 后面的参数会传给TransformComponent的构造函数

		// entt提供的Callback, 当TransformComponent被创建时, 调用OnTransformConstruct函数
		m_Registry.on_construct<TransformComponent>().connect<&OnTransformConstruct>();

		// 判断entity上是否有TransformComponent, 相当于HasComponent
		if (m_Registry.all_of<TransformComponent>(entity))
			// 从entity上get TransformComponent, 相当于GetComponent
			TransformComponent& transform = m_Registry.get<TransformComponent>(entity);

		// 获取所有带有TransformComponent的entity数组
		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		// group用来获取同时满足拥有多个Component的Entity数组, 这里得到的group是
		// m_Registry里所有既有TransformComponent、又有MeshComponent的Entity数组
		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		// 这样写行不行?
		//auto group = m_Registry.group<TransformComponent, MeshComponent>();
		for (auto entity : group)
		{
			// transform和mesh都是记录的引用
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity); // 之前报错时因为MeshComponent是空的
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
		Entity entity = { m_Registry.create(), this };  //这里m_EntityHandle = 0了（因为create（）返回了0）
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
	// 在Flame里写了个模板类，然后再在EditorLayer里调用这个模板类，
	// 但问题在于，目前在引擎内部没有调用到AddComponent代码，导致该模板没有被生成出来，此时EditorLayer调用OnComponentAdded会失败，所以需要手动让该OnComponentAdded可以编译。
	// 不过正常情况下不需要用到这个功能，正常情况下，模板应该被定义在header文件里
	// 注意, 这里不是模板特化, 而是让编译器生成这几个特定类型的模板函数而已   ?

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