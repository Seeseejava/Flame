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
		// ע��RenderCommand��ĺ�����Ӧ���ǵ�һ���ܵĺ�������Ӧ����������ϵ��κι���
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexcount = 0)
		{
			// �������ﲻ���Ե���vertexArray->Bind()����
			s_RendererAPI->DrawIndexed(vertexArray, indexcount);
		}

		inline static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		inline static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}