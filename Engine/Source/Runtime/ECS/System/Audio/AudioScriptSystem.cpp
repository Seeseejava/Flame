#include "flamepch.h"

#include "Runtime/ECS/System/Audio/AudioScriptSystem.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"

namespace Flame
{
	AudioScriptSystem::AudioScriptSystem(Scene* scene)
		: System(scene)
	{
	}

	void AudioScriptSystem::OnRuntiemStart()
	{
		FMOD::System_Create(&mFmodSystem);
		mFmodSystem->init(32, FMOD_INIT_NORMAL, 0);

		mFmodSystem->createSound(AssetManager::GetFullPath("Assets/Audios/singing.wav").string().c_str(), FMOD_DEFAULT, 0, &mSound1);
	}

	void AudioScriptSystem::OnUpdateRuntime(Timestep ts)
	{
		mFmodSystem->playSound(mSound1, 0, false, &mChannel);

		mFmodSystem->update();
	}

	void AudioScriptSystem::OnRuntimeStop()
	{
		mSound1->release();

		mFmodSystem->close();
		mFmodSystem->release();
	}

	void AudioScriptSystem::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{

	}
}