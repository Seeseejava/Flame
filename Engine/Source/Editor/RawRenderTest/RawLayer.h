#pragma once

#include "Flame.h"

namespace Flame
{
	class RawLayer : public Layer
	{
	public:
		void OnUpdate(Timestep ts) override;
	};
}