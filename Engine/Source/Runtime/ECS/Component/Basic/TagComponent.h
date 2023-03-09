#pragma once
#include "Runtime/ECS/Component/ComponentBase.h"

#include <string>

namespace Flame
{
	class TagComponent : public ComponentBase
	{
	public:
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};
}