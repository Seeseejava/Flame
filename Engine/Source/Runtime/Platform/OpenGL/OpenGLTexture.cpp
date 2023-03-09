#include "flamepch.h"
#include "OpenGLTexture.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>



namespace Flame {

	// ---------------Tex2D--------------------
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
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

		if (data)
		{
			m_IsLoaded = true;

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
			else if (channels == 1)
			{
				internalFormat = GL_RGB;
				dataFormat = GL_UNSIGNED_BYTE;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			//FLAME_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, type, data);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

			stbi_image_free(data);
		}
		else
		{
			throw "Load Texture Failed!";
		}
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


	void OpenGLTexture2D::Bind(uint32_t slot) const 
	{
		FLAME_PROFILE_FUNCTION();
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		//glBindTextureUnit(slot, m_RendererID);//(unit, texture)
	}

	void OpenGLTexture2D::UnBind() const
	{
		glBindTexture(GL_TEXTURE, 0);
	}

	// ---------------Tex3D--------------------
   // TODO
	OpenGLTexture3D::OpenGLTexture3D(uint32_t width, uint32_t height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	OpenGLTexture3D::OpenGLTexture3D(uint32_t rendererID, uint32_t width, uint32_t height)
		: m_RendererID(rendererID), m_Width(width), m_Height(height)
	{
	}


	OpenGLTexture3D::~OpenGLTexture3D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture3D::Bind(uint32_t slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID);
	}

	void OpenGLTexture3D::UnBind() const
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	// ---------------CubeMap--------------------
	OpenGLCubeMapTexture::OpenGLCubeMapTexture()
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		m_Width = 512;
		m_Height = 512;

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	OpenGLCubeMapTexture::OpenGLCubeMapTexture(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	// refer to https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/
	OpenGLCubeMapTexture::OpenGLCubeMapTexture(std::vector<std::string>& paths)
		: m_Paths(paths)
	{
		glGenTextures(1, &m_RendererID);
		OpenGLCubeMapTexture::Generate();
	}

	OpenGLCubeMapTexture::~OpenGLCubeMapTexture()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLCubeMapTexture::SetFace(FaceTarget faceIndex, const std::string& path)
	{
		m_Paths[(uint32_t)faceIndex] = path;
	}

	void OpenGLCubeMapTexture::GenerateMipmap()
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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

	void OpenGLCubeMapTexture::Generate()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width = 0, height = 0, nrChannels = 0;
		stbi_set_flip_vertically_on_load(false);

		for (unsigned int i = 0; i < m_Paths.size(); i++)
		{
			unsigned char* data = stbi_load(AssetManager::GetFullPath(m_Paths[i]).string().c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
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
}