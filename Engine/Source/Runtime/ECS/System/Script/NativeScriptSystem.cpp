#include "flamepch.h"
//Ä¿Ç°²»¶®

#include "Runtime/ECS/System/Script/NativeScriptSystem.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/ECS/Entity/Entity.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/ScriptableEntity.h"

namespace Flame
{
	void NativeScriptSystem::OnUpdate(Timestep ts)
	{
		if (!ModeManager::IsEditState())
		{
			m_Scene->m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)  // nsc: native script component
				{
					// TODO: Move to Level::OnScenePlay
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
}