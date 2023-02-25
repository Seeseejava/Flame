#pragma once 

#include "Runtime/ECS/Component/ComponentBase.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Flame
{
	class DirectionalLightComponent : public ComponentBase
	{
	public:
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
		DirectionalLightComponent(const glm::vec3& lightDir)
			: LightDir(lightDir) {}

		glm::vec3 LightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
	};
}