#include "flamepch.h"

#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"

namespace Flame
{
    ShaderLibrary::ShaderLibrary()
    {
        Add("Common", AssetManager::GetFullPath("Shaders/Common.glsl"));
        Add("BasePBR", AssetManager::GetFullPath("Shaders/BasePBR.glsl"));
    }

    Ref<Shader> ShaderLibrary::GetDefaultShader()
    {
        return ShaderLibrary::GetInstance().Get("BasePBR");
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        FLAME_CORE_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const std::filesystem::path& shaderPath)
    {
        Add(Shader::Create(shaderPath));
    }

    void ShaderLibrary::Add(const std::string& name, const std::filesystem::path& shaderPath)
    {
        Add(name, Shader::Create(shaderPath));
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        auto& name = shader->GetName();
        Add(name, shader);
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
    {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        FLAME_CORE_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.find(name) != m_Shaders.end();
    }
}