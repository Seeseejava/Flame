#pragma once 

#include "Runtime/Library/Library.h"
#include "Runtime/Renderer/UniformBuffer.h"

#include <unordered_map>

#include <glm/glm.hpp>

namespace Flame
{
    struct CameraData
    {
    public:
        glm::mat4 ViewProjection;
    };

    template<>
    class Library<UniformBuffer> : public LibraryBase<Library, UniformBuffer>
    {
    public:
        Library();
        Ref<UniformBuffer> GetCameraUniformBuffer();
    };

}