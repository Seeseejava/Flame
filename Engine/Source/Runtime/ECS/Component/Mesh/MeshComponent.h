#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/Mesh/Mesh.h"

#include <filesystem>
namespace Flame
{
	class MeshComponent : public ComponentBase
	{
	public:
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: Path(path), Mesh(path)
		{
		}

		Mesh Mesh;
		std::string Path;
	};
}