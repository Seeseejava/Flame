#pragma once

#include <glm/glm.hpp>
#include <array>

namespace Flame::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

	glm::mat4 CalculateCovMatrix(const std::vector<glm::vec3>& vertices);
}