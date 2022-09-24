#include "flamepch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace Flame {

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!")
				return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexArray();
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}