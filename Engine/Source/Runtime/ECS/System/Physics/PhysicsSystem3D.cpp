#include "flamepch.h"

#include "Runtime/ECS/System/Physics/PhysicsSystem3D.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

namespace Flame
{
	void PhysicsSystem3D::OnRuntiemStart()
	{
		mBroadphase = new btDbvtBroadphase();
		mCollisionConfiguration = new btDefaultCollisionConfiguration();
		mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
		mSolver = new btSequentialImpulseConstraintSolver();
		mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
		mDynamicsWorld->setGravity(btVector3(0.0, -10.0, 0.0));
		mDynamicsWorld->setForceUpdateAllAabbs(true);
	}

	void PhysicsSystem3D::OnUpdateRuntime(Timestep ts)
	{
	}

	void PhysicsSystem3D::OnRuntimeStop()
	{
		delete mDynamicsWorld;
		delete mSolver;
		delete mDispatcher;
		delete mCollisionConfiguration;
		delete mBroadphase;
	}
}