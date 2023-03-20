#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"
#include "Runtime/Renderer/UniformBuffer.h"

namespace Flame
{


	class EnvironmentSystem : public System
	{
	public:
		EnvironmentSystem(Scene* scene) : System(scene) {}
		virtual ~EnvironmentSystem() = default;
	public:
		void OnUpdateRuntime(Timestep ts) override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	private:
		void DrawSkyBox(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix);
	};
}
