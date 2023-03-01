#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

#include <fmod.hpp>

namespace Flame
{
	class AudioScriptSystem : public System
	{
	public:
		AudioScriptSystem(Scene* scene);
		virtual ~AudioScriptSystem() = default;
	public:
		void OnRuntiemStart() override;
		void OnUpdateRuntime(Timestep ts) override;
		void OnRuntimeStop() override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	private:
		FMOD::System* mFmodSystem;
		FMOD::Sound* mSound1;
		FMOD::Channel* mChannel = 0;
	};
}