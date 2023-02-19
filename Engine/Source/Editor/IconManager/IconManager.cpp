#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Editor/IconManager/IconManager.h"

namespace Flame
{
    IconManager::IconManager()
    {
        Add("EntityIcon", Texture2D::Create(AssetManager::GetFullPath("Resources/Icon/Entity.png")));//Ϊʲô����Ҫgetinstance��һ���ˣ�
		Add("NullTextrue", Texture2D::Create(AssetManager::GetFullPath("Assets/texture/Checkerboard.png")));
		Add("SettingIcon", Texture2D::Create(std::string(("Resources/Icon/Setting.png"))));
		Add("DirectoryIcon", Texture2D::Create(std::string(("Resources/ContentBrowser/DirectoryIcon.png"))));
		Add("FileIcon", Texture2D::Create(std::string(("Resources/ContentBrowser/FileIcon.png"))));
		Add("PlayIcon", Texture2D::Create(std::string(("Resources/ToolBar/PlayButton.png"))));
		Add("StopIcon", Texture2D::Create(std::string(("Resources/ToolBar/StopButton.png"))));
    }
}