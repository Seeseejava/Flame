#include "flamepch.h"

#include "Runtime/Library/UniformBufferLibrary.h"

#include <glm/glm.hpp>

namespace Flame
{

    Library<UniformBuffer>::Library()
    {
        Ref<UniformBuffer> CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
        Add("CameraUniform", CameraUniformBuffer);
    }


    Ref<UniformBuffer> Library<UniformBuffer>::GetCameraUniformBuffer()
    {
        return m_Library["CameraUniform"];
    }
}