#include "flamepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace Flame {

	Flame::Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!")
				return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(filepath);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	Shader* Shader::Create(const std::string& vertexsrc, const std::string& fragmentsrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!")
					return nullptr;
			case RendererAPI::API::OpenGL:
				return new OpenGLShader(vertexsrc, fragmentsrc);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}