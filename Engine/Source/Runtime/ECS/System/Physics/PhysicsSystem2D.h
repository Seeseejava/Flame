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
		void OnRuntiemStart() override;
		void OnUpdate(Timestep ts) override;
		void OnRuntimeStop() override;
	private:
		class b2World* m_PhysicsWorld = nullptr;
	};
}