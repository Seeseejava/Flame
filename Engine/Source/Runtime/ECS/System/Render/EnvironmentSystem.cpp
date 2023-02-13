#include "flamepch.h"

#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/ECS/System/Render/EnvironmentSystem.h"
#include "Runtime/ECS/Component/ComponentGroup.h"
#include "Runtime/ECS/Entity/Entity.h"

#include "Runtime/Renderer/Renderer3D.h"

namespace Flame
{
	void EnvironmentSystem::OnUpdateRuntime(Timestep ts)
	{

	}

	void EnvironmentSystem::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		switch (ModeManager::m_SceneMode)
		{
		case SceneMode::None:
			break;
		case SceneMode::EnvironmentHdr:
			break;
		case SceneMode::SkyBox:
			Renderer3D::DrawSkyBox(camera);
			break;
		default:
			break;
		}
	}

	void EnvironmentSystem::DrawSkyBox(const glm::mat4& ViewMatrix, const glm::mat4& ProjectMatrix, Ref<UniformBuffer> CameraUniformBuffer)
	{

	}
}