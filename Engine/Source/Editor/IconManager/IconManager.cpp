#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Editor/IconManager/IconManager.h"

namespace Flame
{
    IconManager::IconManager()
    {
        m_NullTexture = Texture2D::Create(AssetManager::GetFullPath("Assets/texture/Checkerboard.png"));
        m_DirectoryIcon = Texture2D::Create(AssetManager::GetFullPath("Resources/ContentBrowser/DirectoryIcon.png"));
        m_FileIcon = Texture2D::Create(AssetManager::GetFullPath("Resources/ContentBrowser/FileIcon.png"));
        m_SettingIcon = Texture2D::Create(AssetManager::GetFullPath("Resources/Icon/Setting.png"));//为什么不需要getinstance这一步了？
    }
}