#include "flamepch.h"

#include "Runtime/Library/Library.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Renderer/Model.h"
#include "UniformBufferLibrary.h"


namespace Flame
{
    template<typename LibType>
    Library<LibType>::Library()
    {
    }

    template<>
    Library<Model>::Library()
    {
        Add("Box", CreateRef<Model>("Assets/Models/Box.obj"));
    }

    template<>
    Library<CubeMapTexture>::Library()
    {
        std::vector<std::string> Paths{
            "Assets/texture/Skybox/right.jpg",
            "Assets/texture/Skybox/left.jpg",
            "Assets/texture/Skybox/top.jpg",
            "Assets/texture/Skybox/bottom.jpg",
            "Assets/texture/Skybox/front.jpg",
            "Assets/texture/Skybox/back.jpg",
        };
        Add("SkyBoxTexture", CubeMapTexture::Create(Paths));

        Add("EnvironmentHdr", CubeMapTexture::Create());
    }
}