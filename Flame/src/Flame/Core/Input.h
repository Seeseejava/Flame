#pragma once

#include "Flame/Core/Core.h"

namespace Flame {

	class FLAME_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePosition();
	};
}