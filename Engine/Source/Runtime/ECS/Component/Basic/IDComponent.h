#pragma once
#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/Core/UUID.h"

namespace Flame
{
	class IDComponent : public ComponentBase
	{
	public:
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id)
			: ID(id)
		{
		}
	};
}