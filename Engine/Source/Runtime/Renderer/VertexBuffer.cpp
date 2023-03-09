#include "flamepch.h"

#include "Runtime/Renderer/VertexBuffer.h"

#include "Runtime/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
namespace Flame {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, VertexBufferUsage usage)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLVertexBuffer>(size, usage);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size, VertexBufferUsage usage)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!")
				return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(vertices, size, usage);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}