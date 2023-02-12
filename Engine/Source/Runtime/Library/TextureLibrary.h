#pragma once 

#include "Runtime/Core/PublicSingleton.h"
#include "Runtime/Renderer/Texture.h"

#include <unordered_map>

namespace Flame
{
    class TextureLibrary : public PublicSingleton<TextureLibrary>
    {
    public:
        TextureLibrary();

        Ref<Texture2D> LoadOrFindTexture(const std::string& name, const std::string& path)
        {
            if (m_TextureLibrary.find(path) != m_TextureLibrary.end())
            {
                return m_TextureLibrary[path];
            }
            m_TextureLibrary[name] = Texture2D::Create(path);
            return m_TextureLibrary[name];
        }

        void AddTexture(const std::string& name, const Ref<Texture2D>& texture);

        [[nodiscard]] Ref<Texture2D> GetTexture(const std::string& name);

        [[nodiscard]] Ref<Texture2D> GetDefaultTexture() { return m_TextureLibrary["DefaultTexture"]; }
    private:
        std::unordered_map<std::string, Ref<Texture2D>> m_TextureLibrary;
    };
}