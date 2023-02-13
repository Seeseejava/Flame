#pragma once

#include "Runtime/Renderer/Shader.h"

#include "Runtime/Library/Library.h"

namespace Flame
{
    /*��ʵ���ǰ�Shader�Ķ�ȡ�ʹ洢λ�ö����䵽ShaderLibrary����
    ShaderLibrary���ʾ��Ǹ���ϣmap��key��shader�����֣�value��shader������*/
    template<>
    class Library<Shader> : public LibraryBase<Library, Shader>
    {
    public:

        Library();

        [[nodiscard]] Ref<Shader> GetDefaultShader();
        [[nodiscard]] Ref<Shader> GetSkyBoxShader();

    };
}