#pragma once

#include <filesystem>

#include "Flame/Renderer/Texture.h"

namespace Flame {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender(bool* pOpen);
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}