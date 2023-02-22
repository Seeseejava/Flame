#include "flamepch.h"

#include "Runtime/Library/Library.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Mesh/Mesh.h"
#include "UniformBufferLibrary.h"


namespace Flame
{
    template<typename LibType>
    Library<LibType>::Library()
    {
    }

    template<>
    Library<Mesh>::Library()
    {
        Add("Box", CreateRef<Mesh>("Assets/Models/Box.obj"));
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

        // 创建出来一坨漆黑
		//Add("EnvironmentHdr", CubeMapTexture::Create(512, 512));
		//Add("EnvironmentIrradiance", CubeMapTexture::Create(32, 32));
		//Add("EnvironmentPrefilter", CubeMapTexture::Create(128, 128));

		Add("EnvironmentHdr", CubeMapTexture::Create(Paths));
		Add("EnvironmentIrradiance", CubeMapTexture::Create(Paths));
		Add("EnvironmentPrefilter", CubeMapTexture::Create(Paths));
    }
}