#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class RenderSystem2D : public System
	{
	public:
		RenderSystem2D(Scene* scene) : System(scene) {}
		virtual ~RenderSystem2D() = default;
	public:
		void OnUpdateRuntime(Timestep ts) override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	};
}