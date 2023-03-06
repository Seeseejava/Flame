#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

#include <fmod.hpp>
#include <fmod.h>

namespace Flame
{
	class AudioScriptSystem : public System
	{
	public:
		AudioScriptSystem(Scene* scene);
		virtual ~AudioScriptSystem() = default;
	public:
		void OnRuntimeStart() override;
		void OnUpdateRuntime(Timestep ts) override;
		void OnRuntimeStop() override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	private:
		FMOD::System* m_FmodSystem;
	};
}