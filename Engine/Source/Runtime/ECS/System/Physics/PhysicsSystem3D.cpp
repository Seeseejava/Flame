#include "flamepch.h"

#include "Runtime/ECS/System/Physics/PhysicsSystem3D.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace Flame
{
	Utils::BulletDrawer PhysicsSystem3D::m_DebugDrawer;

	void PhysicsSystem3D::OnRuntiemStart()
	{
		m_Broadphase = new btDbvtBroadphase();
		m_CollisionConfiguration = new btDefaultCollisionConfiguration();
		m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
		m_Solver = new btSequentialImpulseConstraintSolver();
		m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);
		m_DynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));
		//mDynamicsWorld->setForceUpdateAllAabbs(true);

		auto view = m_Scene->m_Registry.view<TransformComponent, Rigidbody3DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, m_Scene };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb3d = entity.GetComponent<Rigidbody3DComponent>();

			btTransform trans;
			btCollisionShape* shape;
			btVector3 inertia{ 0.0f, 0.0f, 0.0f };

			if (rb3d.Shape == CollisionShape::Box)
			{
				shape = new btBoxShape(btVector3(transform.Scale.x, transform.Scale.y, transform.Scale.z));
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);
				
			}
			else if (rb3d.Shape == CollisionShape::Sphere)
			{
				shape = new btSphereShape(transform.Scale.x);
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);
			}
			else if (rb3d.Shape == CollisionShape::ConvexHull && entity.HasComponent<MeshComponent>())
			{
				auto& meshc = entity.GetComponent<MeshComponent>();

				shape = new btConvexHullShape();

				for (const auto& submesh : meshc.m_Mesh->m_SubMeshes)
				{
					auto& staticVertices = submesh.m_StaticVertices;
					auto& skinnedVertices = submesh.m_SkinnedVertices;
					for (const auto& vertex : staticVertices)
					{
						static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Pos.x * transform.Scale.x, vertex.Pos.y * transform.Scale.y, vertex.Pos.z * transform.Scale.z));
					}
					for (const auto& vertex : skinnedVertices)
					{
						static_cast<btConvexHullShape*>(shape)->addPoint(btVector3(vertex.Pos.x * transform.Scale.x, vertex.Pos.y * transform.Scale.y, vertex.Pos.z * transform.Scale.z));
					}
				}
			}

			if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);
			trans.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));
			auto comQuat = glm::quat(transform.Rotation);
			btQuaternion btQuat;
			btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
			trans.setRotation(btQuat);
			btDefaultMotionState* motion = new btDefaultMotionState(trans);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(rb3d.mass, motion, shape, inertia);
			rbInfo.m_linearDamping = rb3d.linearDamping;
			rbInfo.m_angularDamping = rb3d.angularDamping;
			rbInfo.m_restitution = rb3d.restitution;
			rbInfo.m_friction = rb3d.friction;

			btRigidBody* body = new btRigidBody(rbInfo);
			body->setSleepingThresholds(0.01f, glm::radians(0.1f));
			body->setActivationState(DISABLE_DEACTIVATION);

			if (rb3d.Type == Rigidbody3DComponent::Body3DType::Static)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
			}
			else if (rb3d.Type == Rigidbody3DComponent::Body3DType::Kinematic)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			}
			else if (rb3d.Type == Rigidbody3DComponent::Body3DType::Dynamic)
			{
				body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
			}
			rb3d.RuntimeBody = body;
			m_DynamicsWorld->addRigidBody(body);
		}
	}

	void PhysicsSystem3D::OnUpdateRuntime(Timestep ts)
	{
		m_DynamicsWorld->stepSimulation(ts, 10);

		auto view = m_Scene->m_Registry.view<TransformComponent, Rigidbody3DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, m_Scene };
			auto componentsTuple = entity.GetComponents<TransformComponent, Rigidbody3DComponent>();
			auto [transform, rb3d] = componentsTuple;

			btRigidBody* body = (btRigidBody*)(*rb3d).RuntimeBody;
			const auto& trans = body->getWorldTransform();
			(*transform).Translation.x = trans.getOrigin().x();
			(*transform).Translation.y = trans.getOrigin().y();
			(*transform).Translation.z = trans.getOrigin().z();

			btScalar yawZ, pitchY, rollX;
			trans.getRotation().getEulerZYX(yawZ, pitchY, rollX);

			(*transform).Rotation.x = rollX;
			(*transform).Rotation.y = pitchY;
			(*transform).Rotation.z = yawZ;
		}

		if (ModeManager::bShowPhysicsColliders)
		{
			Entity camera = m_Scene->GetPrimaryCameraEntity();
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());

			m_DynamicsWorld->setDebugDrawer(&m_DebugDrawer);
			m_DynamicsWorld->debugDrawWorld();

			Renderer2D::EndScene();
		}
	}

	void PhysicsSystem3D::OnRuntimeStop()
	{
		
		delete m_DynamicsWorld;
		delete m_Solver;
		delete m_Dispatcher;
		delete m_CollisionConfiguration;
		delete m_Broadphase;
	}

	void PhysicsSystem3D::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		if (ModeManager::bShowPhysicsColliders)
		{
			// TEMP
			OnRuntiemStart();

			Renderer2D::BeginScene(camera);

			m_DynamicsWorld->setDebugDrawer(&m_DebugDrawer);
			m_DynamicsWorld->debugDrawWorld();

			Renderer2D::EndScene();

			OnRuntimeStop();
		}
	}
}