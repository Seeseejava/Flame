#pragma once

#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Library/TextureLibrary.h"

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
		Material() { Initialize(); };
		Material(Ref<Shader> shader) : m_Shader(shader) { Initialize(); };
    public:
        void SetShader(Ref<Shader> shader) { m_Shader = shader; }
        Ref<Shader> GetShader() { return m_Shader; }

        void AddTexture(TextureType type, Ref<Texture2D> texture) 
        {
            FLAME_CORE_ASSERT(m_TexMap.find(type) == m_TexMap.end(), "Texture already exists");
            m_TexMap[type] = texture;
        }

        Ref<Texture2D> GetTexture(TextureType type) { return m_TexMap[type]; }
	private:
		void Initialize();
    public:
        std::vector<MaterialTexture> m_Textures;

		bool bUseAlbedoMap = false;
		glm::vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f }; // 0 ~ 1
		Ref<Texture2D> albedoRGBA = Texture2D::Create(1, 1);
		Ref<Texture2D> m_AlbedoMap = Library<Texture2D>::GetInstance().GetDefaultTexture();

		bool bUseNormalMap = false;
		Ref<Texture2D> m_NormalMap = Library<Texture2D>::GetInstance().Get("DefaultNormal");

		bool bUseMetallicMap = false;
		float metallic = 0.1f;
		Ref<Texture2D> metallicRGBA = Texture2D::Create(1, 1);
		Ref<Texture2D> m_MetallicMap = Library<Texture2D>::GetInstance().Get("DefaultMetallicRoughness");

		bool bUseRoughnessMap = false;
		float roughness = 0.9f;
		Ref<Texture2D> roughnessRGBA = Texture2D::Create(1, 1);
		Ref<Texture2D> m_RoughnessMap = Library<Texture2D>::GetInstance().Get("DefaultMetallicRoughness");

		bool bUseAoMap = false;
		Ref<Texture2D> m_AoMap = Library<Texture2D>::GetInstance().GetWhiteTexture();
    private:
        Ref<Shader> m_Shader;
        std::unordered_map<TextureType, Ref<Texture2D>, EnumClassHash> m_TexMap;
    };

}