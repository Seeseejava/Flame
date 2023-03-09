#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"

#include <concepts>
#include <type_traits>

namespace Flame
{
	template<class T>
	concept Component = std::is_base_of_v<ComponentBase, T>;//约束必须是ComponentBase的子类
}