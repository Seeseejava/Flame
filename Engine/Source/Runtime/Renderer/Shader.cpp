#include "flamepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/NativeOpenGLShader.h"
namespace Flame {


	Ref<Shader> Shader::Create(const std::filesystem::path& filepath)
	{
		return Create(filepath.string());
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!")
				return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(filepath);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}



	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexsrc, const std::string& fragmentsrc)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!")
					return nullptr;
			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLShader>(name, vertexsrc, fragmentsrc);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::CreateNative(const std::filesystem::path& filepath)
	{
		return CreateNative(filepath.string());
	}

	Ref<Shader> Shader::CreateNative(const std::string& filepath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<NativeOpenGLShader>(filepath);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::CreateNative(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<NativeOpenGLShader>(name, vertexSrc, fragmentSrc);

		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		FLAME_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Flame::Ref<Flame::Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Flame::Ref<Flame::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Flame::Ref<Flame::Shader> ShaderLibrary::Get(const std::string& name)
	{
		FLAME_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

	ShaderUniform::ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
		: mName(name), mType(type), mSize(size), mOffset(offset)
	{
	}
	const std::string& ShaderUniform::UniformTypeToString(ShaderUniformType type)
	{
		if (type == ShaderUniformType::Bool)
		{
			return "Boolean";
		}
		else if (type == ShaderUniformType::Int)
		{
			return "Int";
		}
		else if (type == ShaderUniformType::Float)
		{
			return "Float";
		}
		return "None";
	}
}