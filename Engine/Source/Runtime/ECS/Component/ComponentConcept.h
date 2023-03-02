#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"

#include <concepts>
#include <type_traits>

namespace Flame
{
	template<class T>
	concept Component = std::is_base_of_v<ComponentBase, T>;//Լ��������ComponentBase������
}