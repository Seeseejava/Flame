#include "flamepch.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

namespace Flame
{
	ConfigManager::ConfigManager()
	{
		mRootFolder = { FLAME_XSTRING(ENGINE_ROOT_DIR) };
		std::string s = mRootFolder.string();
		std::string s1 = FLAME_STRING(ENGINE_ROOT_DIR);
		mAssetsFolder = mRootFolder / "Assets";
		mShadersFolder = mRootFolder / "Shaders";
		mResourcesFolder = mRootFolder / "Resources";
	}

	void ConfigManager::Clear()
	{
		mRootFolder.clear();
		mAssetsFolder.clear();
		mShadersFolder.clear();
		mResourcesFolder.clear();
	}

	const std::filesystem::path& ConfigManager::GetRootFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(mRootFolder), "No such file!");
		return mRootFolder;
	}

	const std::filesystem::path& ConfigManager::GetAssetsFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(mAssetsFolder), "No such file!");
		return mAssetsFolder;
	}

	const std::filesystem::path& ConfigManager::GetShadersFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(mShadersFolder), "No such file!");
		return mShadersFolder;
	}

	const std::filesystem::path& ConfigManager::GetResourcesFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(mResourcesFolder), "No such file!");
		return mResourcesFolder;
	}
}