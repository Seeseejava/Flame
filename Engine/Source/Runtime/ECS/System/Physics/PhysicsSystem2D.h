#pragma once

#include "Runtime/ECS/System/System.h"
#include "Runtime/ECS/Scene/Scene.h"

namespace Flame
{
	class PhysicsSystem2D : public System
	{
	public:
		PhysicsSystem2D(Scene* scene) : System(scene) {}
		virtual ~PhysicsSystem2D() = default;
	public:
		void OnRuntimeStart() override;
		void OnUpdateRuntime(Timestep ts) override;
		void OnRuntimeStop() override;
		void OnUpdateEditor(Timestep ts, EditorCamera& camera) override;
	private:
		class b2World* m_PhysicsWorld = nullptr;
	};
}