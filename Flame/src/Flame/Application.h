#pragma once

#include "Core.h"

namespace Flame {

	class FLAME_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}