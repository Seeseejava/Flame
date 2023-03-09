#include "flamepch.h"

#include "Runtime/ECS/System/Audio/AudioScriptSystem.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/ECS/Entity/Entity.h"

namespace Flame
{
	AudioScriptSystem::AudioScriptSystem(Scene* scene)
		: System(scene)
	{
	}

	void AudioScriptSystem::OnRuntimeStart()
	{
		FMOD::System_Create(&m_FmodSystem);
		m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);

		auto view = m_Scene->m_Registry.view<TransformComponent, SoundComponent>();
		for (auto e : view)
		{
			Entity entity = { e, m_Scene };
			auto& sc = entity.GetComponent<SoundComponent>();

			m_FmodSystem->createSound(AssetManager::GetFullPath(sc.Path).string().c_str(), FMOD_DEFAULT, 0, &sc.Sound);

			m_FmodSystem->playSound(sc.Sound, nullptr, false, &sc.Channel);
		}
	}

	void AudioScriptSystem::OnUpdateRuntime(Timestep ts)
	{

		m_FmodSystem->update();
	}

	void AudioScriptSystem::OnRuntimeStop()
	{

		m_FmodSystem->close();
		m_FmodSystem->release();
	}

	void AudioScriptSystem::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{

	}
}