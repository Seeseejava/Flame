#include "flamepch.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Library/UniformBufferLibrary.h"

#include <glm/glm.hpp>

namespace Flame
{
    struct CameraData
    {
        glm::mat4 ViewProjection;
    };

    UniformBufferLibrary::UniformBufferLibrary()
    {
        Ref<UniformBuffer> CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
        AddUniformBuffer("CameraUniform", CameraUniformBuffer);
    }

    void UniformBufferLibrary::AddUniformBuffer(const std::string& name, const Ref<UniformBuffer>& texture)
    {
        FLAME_CORE_ASSERT(m_UniformBufferLibrary.find(name) == m_UniformBufferLibrary.end(), "Uniform already exists!");
        m_UniformBufferLibrary[name] = texture;
    }

    Ref<UniformBuffer> UniformBufferLibrary::GetUniformBuffer(const std::string& name)
    {
        FLAME_CORE_ASSERT(m_UniformBufferLibrary.find(name) != m_UniformBufferLibrary.end(), "No such Uniform!");
        return m_UniformBufferLibrary[name];
    }
}