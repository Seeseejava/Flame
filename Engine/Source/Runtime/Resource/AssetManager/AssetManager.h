#pragma once

#include "Runtime/Core/PublicSingleton.h"

#include <filesystem>

namespace Flame
{
	class AssetManager : public PublicSingleton<AssetManager>
	{
	public:
		std::filesystem::path GetFullPath(const std::string& RelativePath) const;
	};
}