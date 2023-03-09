#include "flamepch.h"

#include "Runtime/ECS/System/Physics/PhysicsSystem3D.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Math/Math.h"

namespace Flame
{
	Utils::BulletDrawer PhysicsSystem3D::m_DebugDrawer;

	void PhysicsSystem3D::OnRuntimeStart()
	{
		//Broad-Phase，使用AABB的BVH快速检测碰撞
		m_Broadphase = new btDbvtBroadphase();
		//碰撞配置，物理世界存在柔体的话，使用btSoftBodyRigidBodyCollisionConfiguration，不存在则使用btDefaultCollisionConfiguration
		m_CollisionConfiguration = new btDefaultCollisionConfiguration();
		//Narrow-Phase，碰撞调度，精确检测碰撞
		m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
		//解算器
		m_Solver = new btSequentialImpulseConstraintSolver();
		//创建物理世界，柔体需要使用btSoftRigidDynamicsWorld
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
				// Calculate Obb
				FLAME_CORE_ASSERT(entity.HasComponent<MeshComponent>(), "No MeshComponent!");
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& mesh = entity.GetComponent<MeshComponent>();

				std::vector<glm::vec3> vertices;
				for (const auto& subMesh : mesh.m_Mesh->m_SubMeshes)
				{
					if (subMesh.m_StaticVertices.empty())
					{
						for (const auto& vertex : subMesh.m_SkinnedVertices)
						{
							vertices.emplace_back(vertex.Pos);
						}
					}
					else
					{
						for (const auto& vertex : subMesh.m_StaticVertices)
						{
							vertices.emplace_back(vertex.Pos);
						}
					}
				}

				glm::vec3 originPos(0.0f);
				glm::mat3 covMat = Math::CalculateCovMatrix(vertices, originPos);

				glm::vec3 eValues;
				glm::mat3 eVectors;
				Math::JacobiSolver(covMat, eValues, eVectors);

				for (int i = 0; i < 3; i++)
				{
					if (eValues[i] == 0 || i == 2)
					{
						Math::SchmidtOrthogonalization(eVectors[(i + 1) % 3], eVectors[(i + 2) % 3], eVectors[i]);
						break;
					}
				}

				constexpr float infinity = std::numeric_limits<float>::infinity();
				glm::vec3 minExtents(infinity, infinity, infinity);
				glm::vec3 maxExtents(-infinity, -infinity, -infinity);

				for (const glm::vec3& displacement : vertices)
				{
					minExtents[0] = std::min(minExtents[0], glm::dot(displacement, eVectors[0]));
					minExtents[1] = std::min(minExtents[1], glm::dot(displacement, eVectors[1]));
					minExtents[2] = std::min(minExtents[2], glm::dot(displacement, eVectors[2]));

					maxExtents[0] = std::max(maxExtents[0], glm::dot(displacement, eVectors[0]));
					maxExtents[1] = std::max(maxExtents[1], glm::dot(displacement, eVectors[1]));
					maxExtents[2] = std::max(maxExtents[2], glm::dot(displacement, eVectors[2]));
				}

				glm::vec3 halfExtent = (maxExtents - minExtents) / 2.0f;
				glm::vec3 offset = halfExtent + minExtents;
				originPos += offset.x * eVectors[0] + offset.y * eVectors[1] + offset.z * eVectors[2];
				glm::vec3 offsetScale = originPos * (transform.Scale - 1.0f);
				originPos += offsetScale;
				originPos = glm::mat3(transform.GetRotationMatrix()) * originPos;
				originPos += transform.Translation;


				shape = new btBoxShape(btVector3(halfExtent.x * transform.Scale.x, halfExtent.y * transform.Scale.y, halfExtent.z * transform.Scale.z));
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				trans.setOrigin(Utils::GlmToBtVec3(originPos));

				auto comQuat = glm::quat(transform.Rotation) * glm::quat(glm::mat4(eVectors));
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				trans.setRotation(btQuat);
			}
			else if (rb3d.Shape == CollisionShape::Sphere)
			{
				shape = new btSphereShape(transform.Scale.x);
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				trans.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));

				auto comQuat = glm::quat(transform.Rotation);
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				trans.setRotation(btQuat);
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
				if (rb3d.mass > 0.0f) shape->calculateLocalInertia(rb3d.mass, inertia);

				trans.setOrigin(btVector3(transform.Translation.x, transform.Translation.y, transform.Translation.z));

				auto comQuat = glm::quat(transform.Rotation);
				btQuaternion btQuat;
				btQuat.setValue(comQuat.x, comQuat.y, comQuat.z, comQuat.w);
				trans.setRotation(btQuat);
			}

			btDefaultMotionState* motion = new btDefaultMotionState(trans);

			btRigidBody::btRigidBodyConstructionInfo rbInfo(rb3d.mass, motion, shape, inertia);
			rbInfo.m_linearDamping = rb3d.linearDamping;
			rbInfo.m_angularDamping = rb3d.angularDamping;
			rbInfo.m_restitution = rb3d.restitution;
			rbInfo.m_friction = rb3d.friction;

			btRigidBody* body = new btRigidBody(rbInfo);
			body->setSleepingThresholds(0.01f, glm::radians(0.1f));
			body->setActivationState(DISABLE_DEACTIVATION);

			//setCollisionFlags可以设置一些碰撞flag，
			//Bullet3默认(dynamic)会进行物理计算会调用motionState的get和set刚体Transform，
			//把flag设置成btCollisionObject::CF_STATIC_OBJECT后，motionState的get和set都不会一直被调用，只在初始化时被调用，
			//flag设置成btCollisionObject::CF_KINEMATIC_OBJECT，motionState的set的不会被调用，get会被调用。
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
		static bool initFlag = true;
		if (ModeManager::bShowPhysicsColliders)
		{
			OnRuntimeStart();

			Renderer2D::BeginScene(camera);

			m_DynamicsWorld->setDebugDrawer(&m_DebugDrawer);
			m_DynamicsWorld->debugDrawWorld();

			Renderer2D::EndScene();
			OnRuntimeStop();
		}
		else
		{

		}
	}
}