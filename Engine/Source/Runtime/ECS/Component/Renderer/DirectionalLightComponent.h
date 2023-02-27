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
		DirectionalLightComponent(float intensity)
			: Intensity(intensity) {}

		float Intensity = 1.0f;
	};
}