#include "flamepch.h"
#include "Texture.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Runtime/Renderer/Renderer.h"
namespace Flame {

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(width, height);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::filesystem::path& path)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(path);
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		return Create(AssetManager::GetFullPath(path));
	}

	Ref<Texture3D> Texture3D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture3D>(width, height);

		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<CubeMapTexture> CubeMapTexture::Create(std::vector<std::string>& paths)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLCubeMapTexture>(paths);

		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<CubeMapTexture> CubeMapTexture::Create()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLCubeMapTexture>();

		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Flame::CubeMapTexture> CubeMapTexture::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLCubeMapTexture>(width, height);

		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}