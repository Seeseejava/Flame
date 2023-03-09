#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class RenderSystem3D : public System
	{
	public:
		RenderSystem3D(Scene* scene) : System(scene) {}
		virtual ~RenderSystem3D() = default;
	public:
		void OnUpdateRuntime(Timestep ts) override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	};
}