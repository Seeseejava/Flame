#include "flamepch.h"
#include "Runtime/Platform/OpenGL/OpenGLVertexBuffer.h"

#include <glad/glad.h>

namespace Flame {

	static GLenum OpenGLUsage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case VertexBufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
		}
		FLAME_CORE_ASSERT(false, "Unknown vertex buffer usage");
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
		:m_Usage(usage)
	{
		FLAME_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, OpenGLUsage(m_Usage));
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size, VertexBufferUsage usage)
		:m_Usage(usage)
	{
		FLAME_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, OpenGLUsage(m_Usage));
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		FLAME_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		FLAME_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

	}

	void OpenGLVertexBuffer::Unbind() const
	{
		FLAME_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}
}