#pragma once

#include "Runtime/Renderer/Shader.h"

#include "Runtime/Library/Library.h"

namespace Flame
{
    /*其实就是把Shader的读取和存储位置都分配到ShaderLibrary类里
    ShaderLibrary本质就是个哈希map，key是shader的名字，value是shader的内容*/
    template<>
    class Library<Shader> : public LibraryBase<Library, Shader>
    {
    public:

        Library();

        [[nodiscard]] Ref<Shader> GetDefaultShader();
        [[nodiscard]] Ref<Shader> GetSkyBoxShader();

    };
}