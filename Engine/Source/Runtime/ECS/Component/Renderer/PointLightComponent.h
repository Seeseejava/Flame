#pragma once 

#include "Runtime/ECS/Component/ComponentBase.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Flame
{
	class PointLightComponent : public ComponentBase
	{
	public:
		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
		PointLightComponent(const glm::vec3& lightColor)
			: LightColor(lightColor) {}

		glm::vec3 LightColor = { 100.0f, 100.0f, 100.0f };
	};
}