#include "flamepch.h"
#include "Entity.h"

namespace Flame {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}