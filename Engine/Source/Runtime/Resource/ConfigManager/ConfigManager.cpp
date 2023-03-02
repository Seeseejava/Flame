#include "flamepch.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

namespace Flame
{
	glm::vec2 ConfigManager::m_ViewportSize = { 1280.0f, 720.0f };
	int ConfigManager::selectedEntity = -1;

	ConfigManager::ConfigManager()
	{
		m_RootFolder = { FLAME_XSTRING(ENGINE_ROOT_DIR) };
		std::string s = m_RootFolder.string();
		std::string s1 = FLAME_STRING(ENGINE_ROOT_DIR);
		m_AssetsFolder = m_RootFolder / "Assets";
		m_ConfigFolder = m_RootFolder / "Config";
		m_ShadersFolder = m_RootFolder / "Shaders";
		m_ResourcesFolder = m_RootFolder / "Resources";
	}

	void ConfigManager::Clear()
	{
		m_RootFolder.clear();
		m_AssetsFolder.clear();
		m_ShadersFolder.clear();
		m_ResourcesFolder.clear();
	}

	const std::filesystem::path& ConfigManager::GetRootFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(m_RootFolder), "No such file!");
		return m_RootFolder;
	}

	const std::filesystem::path& ConfigManager::GetAssetsFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(m_AssetsFolder), "No such file!");
		return m_AssetsFolder;
	}

	const std::filesystem::path& ConfigManager::GetConfigFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(m_ConfigFolder), "No such file!");
		return m_ConfigFolder;
	}

	const std::filesystem::path& ConfigManager::GetShadersFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(m_ShadersFolder), "No such file!");
		return m_ShadersFolder;
	}

	const std::filesystem::path& ConfigManager::GetResourcesFolder() const
	{
		FLAME_CORE_ASSERT(std::filesystem::exists(m_ResourcesFolder), "No such file!");
		return m_ResourcesFolder;
	}
}