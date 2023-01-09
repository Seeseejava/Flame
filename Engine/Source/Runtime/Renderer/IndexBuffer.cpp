#include "flamepch.h"

#include "Runtime/Renderer/IndexBuffer.h"
#include "Runtime/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
namespace Flame {

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(count);
		}
		FLAME_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}