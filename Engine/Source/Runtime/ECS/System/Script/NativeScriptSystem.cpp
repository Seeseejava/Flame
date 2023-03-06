#include "flamepch.h"
//Ŀǰ����

#include "Runtime/ECS/System/Script/NativeScriptSystem.h"
#include "Runtime/ECS/Entity/Entity.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/ScriptableEntity.h"

namespace Flame
{
	void NativeScriptSystem::OnUpdateRuntime(Timestep ts)
	{
		
		m_Scene->m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)  // nsc: native script component
		{
			// TODO: Move to Scene::OnScenePlay
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ entity, m_Scene };
				nsc.Instance->OnCreate();
			}

		nsc.Instance->OnUpdate(ts);
		});
		
	}
}