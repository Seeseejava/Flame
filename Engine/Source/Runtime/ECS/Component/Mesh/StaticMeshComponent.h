#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/Renderer/Model.h"

#include <filesystem>
namespace Flame
{
	class StaticMeshComponent : public ComponentBase
	{
	public:
		StaticMeshComponent() = default;
		StaticMeshComponent(const StaticMeshComponent&) = default;
		StaticMeshComponent(const std::string& path)
			: Path(path)
		{
		}

		StaticMeshComponent(const std::filesystem::path& path)
			: Path(path)
		{
		}


		Model Mesh;
		std::filesystem::path Path;
	};
}