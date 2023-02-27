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
		PointLightComponent(float intensity, const glm::vec3& lightColor)
			: Intensity(intensity), LightColor(lightColor) {}

		float Intensity = 100.0f;
		glm::vec3 LightColor = { 1.0f, 1.0f, 1.0f };
	};
}