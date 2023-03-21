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
		// 缓存从offset字节处开始需要使用地址为data 大小为size的数据块进行更新 如果offset和data综合超出缓存对象绑定的数据范围则会返回一个错误
		glNamedBufferSubData(m_RendererID, offset, size, data); // = glBufferSubData
	}
}