#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"

namespace Flame
{
	class PythonScriptComponent : public ComponentBase
	{
	public:
		PythonScriptComponent() = default;
		PythonScriptComponent(const PythonScriptComponent&) = default;
		PythonScriptComponent(const std::string& path)
			: Path(path)
		{}

		std::string Path;
	};
}