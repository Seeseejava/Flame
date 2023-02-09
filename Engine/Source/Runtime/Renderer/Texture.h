#pragma once

#include <string>

#include "Runtime/Core/Core.h"
#include <filesystem>
namespace Flame
{
	// Texture 可以分为多种类型, 比如CubeTexture, Texture2D
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) {};

		virtual void Bind(uint32_t slot = 0) const = 0;
		virtual void UnBind() const = 0;

		virtual bool IsLoaded() const { return false; };

		virtual bool operator==(const Texture& other) const = 0;
	};

	// 注意, 这里额外包了个Texture2D类, 继承于Texture类
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::filesystem::path& path);
		static Ref<Texture2D> Create(const std::string& path);
	};


	class CubeMapTexture : public Texture
	{
	public:
		static Ref<CubeMapTexture> Create(std::vector<std::string>& paths);
	};
	
}
