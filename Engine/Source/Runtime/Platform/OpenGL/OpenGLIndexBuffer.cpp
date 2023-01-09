#include "flamepch.h"
#include "Runtime/Platform/OpenGL/OpenGLIndexBuffer.h"

#include <glad/glad.h>

namespace Flame {

	
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		FLAME_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		FLAME_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		FLAME_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

	}

	void OpenGLIndexBuffer::Unbind() const
	{
		FLAME_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t count)
	{
		m_Count = count;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint32_t) * count, data); //用来更新一个已有缓冲区对象中的一部分数据
	}
}