#include "flamepch.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Library/TextureLibrary.h"

namespace Flame
{
    TextureLibrary::TextureLibrary()
    {
        LoadOrFindTexture("DefaultTexture", "Assets/texture/checkerboard.png");
        LoadOrFindTexture("DefaultNormal", "Assets/texture/DefaultNormal.png");
        LoadOrFindTexture("DefaultMetallicRoughness", "Assets/texture/DefaultMetallicRoughness.png");

        Ref<Texture2D> whiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
        AddTexture("WhiteTexture", whiteTexture);
    }

    void TextureLibrary::AddTexture(const std::string& name, const Ref<Texture2D>& texture)
    {
        FLAME_CORE_ASSERT(m_TextureLibrary.find(name) == m_TextureLibrary.end(), "No such texture!");
        m_TextureLibrary[name] = texture;
    }

    Ref<Texture2D> TextureLibrary::GetTexture(const std::string& name)
    {
        FLAME_CORE_ASSERT(m_TextureLibrary.find(name) != m_TextureLibrary.end(), "No such texture!");
        return m_TextureLibrary[name];
    }
}