#pragma once

#include "RendererAPI.h"

namespace Flame {

	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		// 注意RenderCommand里的函数都应该是单一功能的函数，不应该有其他耦合的任何功能
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			// 比如这里不可以调用vertexArray->Bind()函数
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}