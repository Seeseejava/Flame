#pragma once 

#include "Runtime/Core/PublicSingleton.h"
#include "Runtime/Renderer/Texture.h"

#include "Runtime/Library/Library.h"
#include <unordered_map>

namespace Flame
{
    template<>
    class Library<Texture2D> : public LibraryBase<Library, Texture2D>
    {
    public:
        Library();

        Ref<Texture2D> GetDefaultTexture();
        Ref<Texture2D> GetWhiteTexture();
    };
}