#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Editor/IconManager/IconManager.h"

namespace Flame
{
    IconManager::IconManager()
    {
        Add("EntityIcon", Texture2D::Create(AssetManager::GetFullPath("Resource/Icon/Entity.png")));//为什么不需要getinstance这一步了？
		Add("NullTextrue", Texture2D::Create(AssetManager::GetFullPath("Asset/Texture/Checkerboard.png")));
		Add("SettingIcon", Texture2D::Create(std::string(("Resource/Icon/Setting.png"))));
		Add("DirectoryIcon", Texture2D::Create(std::string(("Resource/ContentBrowser/DirectoryIcon.png"))));
		Add("FileIcon", Texture2D::Create(std::string(("Resource/ContentBrowser/FileIcon.png"))));
		Add("PlayIcon", Texture2D::Create(std::string(("Resource/ToolBar/PlayButton.png"))));
		Add("StopIcon", Texture2D::Create(std::string(("Resource/ToolBar/StopButton.png"))));
		Add("TransIcon", Texture2D::Create(std::string(("Resource/Icon/Trans.png"))));
		Add("RotateIcon", Texture2D::Create(std::string(("Resource/Icon/Rotate.png"))));
		Add("ScaleIcon", Texture2D::Create(std::string(("Resource/Icon/Scale.png"))));
		Add("MusicIcon", Texture2D::Create(std::string(("Resource/Icon/Music.png"))));
    }
}