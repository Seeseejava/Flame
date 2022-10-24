#pragma once

#include "Flame/Core/core.h"

namespace Flame {

	class FLAME_API GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}