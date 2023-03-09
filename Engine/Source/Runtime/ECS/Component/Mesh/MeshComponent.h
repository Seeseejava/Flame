#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/Mesh/Mesh.h"

#include <filesystem>
namespace Flame
{
	class MeshComponent : public ComponentBase
	{
	public:
		MeshComponent() { m_Mesh = CreateRef<Mesh>(); };
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path)
			: Path(path), m_Mesh(CreateRef<Mesh>(path))
		{
		}

		Ref<Mesh> m_Mesh;
		std::string Path = "None";
	};
}