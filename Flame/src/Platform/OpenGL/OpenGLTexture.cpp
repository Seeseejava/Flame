#include "flamepch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Flame {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);//stbi_uc*是unsigned char,这里为什么期待的通道数是0
		FLAME_CORE_ASSERT(data, "Failed to load image!");

		m_Width = width;
		m_Height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);//(target, number, texture)
		glTextureStorage2D(m_RendererID, 1, GL_RGB8, m_Width, m_Height);//(texture, levels, internalformat,with,height)

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, data);
		//(target, level, xoffset, yoffset, width, height, format, type, const GLvoid* data)
		//此时已传送到GPU

		stbi_image_free(data);



	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);//(unit, texture)
	}

}