#pragma once

#include "Flame/core.h"

namespace Flame {

	class FLAME_API GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}