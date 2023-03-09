#pragma once

#include "entt.hpp"


#include "Runtime/Core/Timestep.h"
#include "Runtime/Camera/EditorCamera.h"
#include "Runtime/Core/UUID.h"

class b2World;

namespace Flame {

	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		void ChangeDimMode();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	public:
		entt::registry m_Registry;
	private:
		std::vector<Scope<class System>> m_Systems;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}