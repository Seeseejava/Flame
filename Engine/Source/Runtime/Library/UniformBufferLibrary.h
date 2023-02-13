#pragma once 

#include "Runtime/Core/PublicSingleton.h"
#include "Runtime/Renderer/UniformBuffer.h"

#include <unordered_map>

namespace Flame
{
    class UniformBufferLibrary : public PublicSingleton<UniformBufferLibrary>
    {
    public:
        UniformBufferLibrary();

        void AddUniformBuffer(const std::string& name, const Ref<UniformBuffer>& texture);

        [[nodiscard]] Ref<UniformBuffer> GetUniformBuffer(const std::string& name);

        [[nodiscard]] Ref<UniformBuffer> GetCameraUniformBuffer() { return m_UniformBufferLibrary["CameraUniform"]; }
    private:
        std::unordered_map<std::string, Ref<UniformBuffer>> m_UniformBufferLibrary;
    };
}