#pragma once

#include "Runtime/Core/PublicSingleton.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Flame
{
	class ConfigManager final : public PublicSingleton<ConfigManager>
	{
		//friend class PublicSingleton<ConfigManager>;

	public:
		ConfigManager();
		ConfigManager(const ConfigManager&) = delete;
		ConfigManager& operator=(const ConfigManager&) = delete;

	public:

		void Clear();

		const std::filesystem::path& GetRootFolder() const;
		const std::filesystem::path& GetAssetsFolder() const;
		const std::filesystem::path& GetShadersFolder() const;
		const std::filesystem::path& GetResourcesFolder() const;

		static glm::vec2 m_ViewportSize;
		static int selectedEntity;
	private:
		std::filesystem::path mRootFolder;
		std::filesystem::path mAssetsFolder;
		std::filesystem::path mShadersFolder;
		std::filesystem::path mResourcesFolder;
	};
}