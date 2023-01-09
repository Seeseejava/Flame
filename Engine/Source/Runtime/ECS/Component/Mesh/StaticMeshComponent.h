#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/Renderer/Model.h"

namespace Flame
{
	class StaticMeshComponent : public ComponentBase
	{
	public:
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;
		StaticMeshComponent(const std::string& Path)
		{
			memcpy(path, Path.c_str(), sizeof(char) * Path.size());
			path[Path.size()] = '\0';
		}

		Model mesh;
		//std::string path;

		char path[256];
	};
}