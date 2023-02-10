#include "flamepch.h"
#include "OpenGLTexture.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>



namespace Flame {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		FLAME_PROFILE_FUNCTION();

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//此处会获取一个RendererID

		// 注意格式是GL_RGBA8不是GL_RGBA, RGBA8是InternalFormat
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path)
		:m_Path(path)
	{
		FLAME_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			FLAME_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std:string&)");
			data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);//stbi_uc*是unsigned char,这里为什么期待的通道数是0
		}
		FLAME_CORE_ASSERT(data, "Failed to load image!");

		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		FLAME_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//(target, number, texture)
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);//(texture, levels, internalformat,with,height)

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		//(target, level, xoffset, yoffset, width, height, format, type, const GLvoid* data)
		//此时已传送到GPU

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		FLAME_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		FLAME_PROFILE_FUNCTION();

		// bpp: bytes per pixel
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		FLAME_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		//可以通过glTextureSubImage2D这个API，为Texture手动提供数据，创建这个WhiteTexture
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLCubeMapTexture::SetFace(FaceTarget faceIndex, const std::string& path)
	{
		m_Paths[(uint32_t)faceIndex] = path;
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const 
	{
		FLAME_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);//(unit, texture)
	}

	void OpenGLTexture2D::UnBind() const
	{
		glBindTexture(GL_TEXTURE, 0);
	}

	// refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(std::vector<std::string>& paths)
		: m_Paths(paths)
	{
		glDeleteTextures(1, &m_RendererID);
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < paths.size(); i++)
		{
			unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				FLAME_CORE_ERROR("Cubemap don't loaded correctly!");
				stbi_image_free(data);
			}
		}

		m_Width = width;
		m_Height = height;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubeMapTexture::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLCubeMapTexture::UnBind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}