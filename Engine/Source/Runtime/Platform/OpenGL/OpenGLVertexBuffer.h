#pragma once

#include "Runtime/Renderer/VertexBuffer.h"

namespace Flame {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		OpenGLVertexBuffer(float* vertices, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		VertexBufferUsage m_Usage;
		BufferLayout m_Layout;
	};

	
}