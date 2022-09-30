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

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		// ע��RenderCommand��ĺ�����Ӧ���ǵ�һ���ܵĺ�������Ӧ����������ϵ��κι���
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			// �������ﲻ���Ե���vertexArray->Bind()����
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}