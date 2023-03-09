#include "flamepch.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

namespace Flame
{
	std::filesystem::path AssetManager::GetFullPath(const std::string& RelativePath)
	{
		return ConfigManager::GetInstance().GetRootFolder() / RelativePath;
	}
}