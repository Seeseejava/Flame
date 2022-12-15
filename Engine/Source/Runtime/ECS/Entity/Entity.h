#pragma once

#include "Runtime/Core/UUID.h"
#include "Runtime/ECS/Scene/Scene.h"
#include "Runtime/ECS/Component/ComponentGroup.h"

#include "entt.hpp"

#include <tuple>
#include <type_traits>

namespace Flame {

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;


		// Ӧ�÷��ش�����Component, ģ�庯����Ӧ�÷ŵ�.h�ļ���
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			FLAME_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...); //�е���������
			m_Scene->OnComponentAdded<T>(*this, component);
			return component; 
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			FLAME_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename... T>
		std::tuple<T*...> GetComponents()
		{
			FLAME_CORE_ASSERT((HasComponent<T>() && ...), "Entity does not have component!");
			return std::make_tuple<T*...>((&m_Scene->m_Registry.get<T>(m_EntityHandle))...);
		}

		//template<typename... T>
		//std::tuple<const T* const...> GetConstComponents()
		//{
		//	FLAME_CORE_ASSERT((HasComponent<T>() && ...), "Entity does not have component!");
		//	return std::make_tuple(((const T* const)&m_Scene->m_Registry.get<T>(m_EntityHandle))...);
		//}


		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			FLAME_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		// TODO:����Ϊweak_ptr
		Scene* m_Scene = nullptr;
	};

}