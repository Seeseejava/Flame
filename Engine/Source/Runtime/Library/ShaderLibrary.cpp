#include "flamepch.h"

#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"

namespace Flame
{
    Library<Shader>::Library()
    {
        Add("BasePBR", Shader::CreateNative(AssetManager::GetFullPath("Shader/BasePBR.glsl")));
        Add("SkyBox", Shader::Create(AssetManager::GetFullPath("Shader/SkyBox.glsl")));

        Add("IBL_background", Shader::CreateNative(AssetManager::GetFullPath("Shader/IBL/background.glsl")));
        Add("IBL_brdf", Shader::CreateNative(AssetManager::GetFullPath("Shader/IBL/brdf.glsl")));
        Add("IBL_equirectangularToCubemap", Shader::CreateNative(AssetManager::GetFullPath("Shader/IBL/equirectangularToCubemap.glsl")));
        Add("IBL_irradiance", Shader::CreateNative(AssetManager::GetFullPath("Shader/IBL/irradiance.glsl")));
        Add("IBL_prefilter", Shader::CreateNative(AssetManager::GetFullPath("Shader/IBL/prefilter.glsl")));

        Add("Post_Outline", Shader::CreateNative(AssetManager::GetFullPath("Shader/PostProcessing/Outline.glsl")));
        Add("Post_Cartoon", Shader::CreateNative(AssetManager::GetFullPath("Shader/PostProcessing/Cartoon.glsl")));
		Add("Post_GrayScale", Shader::CreateNative(AssetManager::GetFullPath("Shader/PostProcessing/GrayScale.glsl")));
		Add("Post_GaussianBlur", Shader::CreateNative(AssetManager::GetFullPath("Shader/PostProcessing/GaussianBlur.glsl")));
        Add("Post_FxaaConsole", Shader::CreateNative(AssetManager::GetFullPath("Shader/PostProcessing/FxaaConsole.glsl")));
        Add("NormalOutline", Shader::CreateNative(AssetManager::GetFullPath("Shader/NormalOutline.glsl")));
        Add("NormalOutline_anim", Shader::CreateNative(AssetManager::GetFullPath("Shader/NormalOutline_anim.glsl")));

        Add("SM_Depth", Shader::CreateNative(AssetManager::GetFullPath("Shader/Shadow/SM_Depth.glsl")));
        Add("CSM_Depth", Shader::CreateNative(AssetManager::GetFullPath("Shader/Shadow/CSM_Depth.glsl")));
        Add("Debug_Depth", Shader::CreateNative(AssetManager::GetFullPath("Shader/Shadow/Debug_Depth.glsl")));
    }

    Ref<Shader> Library<Shader>::GetDefaultShader()
    {
        return m_Library["BasePBR"];
    }

    Ref<Shader> Library<Shader>::GetSkyBoxShader()
    {
        return m_Library["SkyBox"];
    }
    
}