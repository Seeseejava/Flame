#pragma once

#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/ShaderLibrary.h"

#include <unordered_map>

namespace Flame
{
    // from https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
    };

    enum class TextureType
    {
        Albedo = 0,
        Normal = 1,
        Specular = 2,
        Metalness = 3,
        Roughness = 4,
        AmbientOcclusion = 5,
        Height = 6,
        Emission = 7,
    };

    struct MaterialTexture
    {
        Ref<Texture2D> texture2d = nullptr;
        TextureType type;
        std::string path;
    };


    class Material
    {
    public:
        Material() = default;
        Material(Ref<Shader> shader) : m_Shader(shader) {}
    public:
        void SetShader(Ref<Shader> shader) { m_Shader = shader; }
        [[nodiscard]] Ref<Shader> GetShader() { return m_Shader; }

        void AddTexture(TextureType type, Ref<Texture2D> texture) 
        {
            FLAME_CORE_ASSERT(m_TexMap.find(type) == m_TexMap.end(), "Texture already exists");
            m_TexMap[type] = texture;
        }

        [[nodiscard]] Ref<Texture2D> GetTexture(TextureType type) { return m_TexMap[type]; }
    public:
        std::vector<MaterialTexture> m_Textures;
    private:
        Ref<Shader> m_Shader;
        std::unordered_map<TextureType, Ref<Texture2D>, EnumClassHash> m_TexMap;
    };

}