#pragma once 

#include "Runtime/Core/PublicSingleton.h"
#include "Runtime/Renderer/Texture.h"

#include <unordered_map>

namespace Flame
{
    class IconManager : public PublicSingleton<IconManager>
    {
    public:
        IconManager();

		Ref<Texture2D> LoadOrFindTexture(const std::string& path)
		{
			if (iconLibrary.find(path) != iconLibrary.end())
			{
				return iconLibrary[path];
			}
			iconLibrary[path] = Texture2D::Create(path);
			return iconLibrary[path];
		}

        [[nodiscard]] Ref<Texture2D> GetNullTexture() { return m_NullTexture; }
        [[nodiscard]] Ref<Texture2D> GetDirectoryIcon() { return m_DirectoryIcon; }
        [[nodiscard]] Ref<Texture2D> GetFileIcon() { return m_FileIcon; }
        [[nodiscard]] Ref<Texture2D> GetSettingIcon() { return m_SettingIcon; }
    private:
        std::unordered_map<std::string, Ref<Texture2D>> iconLibrary;

        Ref<Texture2D> m_NullTexture;
        Ref<Texture2D> m_DirectoryIcon;
        Ref<Texture2D> m_FileIcon;
        Ref<Texture2D> m_SettingIcon;
    };
}