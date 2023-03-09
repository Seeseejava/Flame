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
        Add("Box", CreateRef<Mesh>("Asset/Model/Box.obj"));
    }

    template<>
    Library<CubeMapTexture>::Library()
    {
        std::vector<std::string> Paths{
            "Asset/Texture/Skybox/right.jpg",
            "Asset/Texture/Skybox/left.jpg",
            "Asset/Texture/Skybox/top.jpg",
            "Asset/Texture/Skybox/bottom.jpg",
            "Asset/Texture/Skybox/front.jpg",
            "Asset/Texture/Skybox/back.jpg",
        };
        Add("SkyBoxTexture", CubeMapTexture::Create(Paths));

		std::vector<std::string> blackPaths{
		   "Asset/Texture/black.png",
		   "Asset/Texture/black.png",
		   "Asset/Texture/black.png",
		   "Asset/Texture/black.png",
		   "Asset/Texture/black.png",
		   "Asset/Texture/black.png",
		};
		Add("BlackCubeMap", CubeMapTexture::Create(blackPaths));

         // 创建出来一坨漆黑
		Add("EnvironmentHdr", CubeMapTexture::Create(512, 512));
		Add("EnvironmentIrradiance", CubeMapTexture::Create(32, 32));
		Add("EnvironmentPrefilter", CubeMapTexture::Create(128, 128));

		//Add("EnvironmentHdr", CubeMapTexture::Create(Paths));
		//Add("EnvironmentIrradiance", CubeMapTexture::Create(Paths));
		//Add("EnvironmentPrefilter", CubeMapTexture::Create(Paths));
    }
}