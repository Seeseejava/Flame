#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Editor/IconManager/IconManager.h"

namespace Flame
{
    IconManager::IconManager()
    {
        Add("EntityIcon", Texture2D::Create(AssetManager::GetFullPath("Resources/Icon/Entity.png")));//为什么不需要getinstance这一步了？
		Add("NullTextrue", Texture2D::Create(AssetManager::GetFullPath("Assets/texture/Checkerboard.png")));
		Add("SettingIcon", Texture2D::Create(std::string(("Resources/Icon/Setting.png"))));
		Add("DirectoryIcon", Texture2D::Create(std::string(("Resources/ContentBrowser/DirectoryIcon.png"))));
		Add("FileIcon", Texture2D::Create(std::string(("Resources/ContentBrowser/FileIcon.png"))));
		Add("PlayIcon", Texture2D::Create(std::string(("Resources/ToolBar/PlayButton.png"))));
		Add("StopIcon", Texture2D::Create(std::string(("Resources/ToolBar/StopButton.png"))));
		Add("TransIcon", Texture2D::Create(std::string(("Resources/Icon/Trans.png"))));
		Add("RotateIcon", Texture2D::Create(std::string(("Resources/Icon/Rotate.png"))));
		Add("ScaleIcon", Texture2D::Create(std::string(("Resources/Icon/Scale.png"))));
		Add("MusicIcon", Texture2D::Create(std::string(("Resources/Icon/Music.png"))));
    }
}