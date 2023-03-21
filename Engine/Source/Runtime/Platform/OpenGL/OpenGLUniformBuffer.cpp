#include "flamepch.h"
#include "Runtime/Platform/OpenGL/OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Flame
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		// �����offset�ֽڴ���ʼ��Ҫʹ�õ�ַΪdata ��СΪsize�����ݿ���и��� ���offset��data�ۺϳ����������󶨵����ݷ�Χ��᷵��һ������
		glNamedBufferSubData(m_RendererID, offset, size, data); // = glBufferSubData
	}
}