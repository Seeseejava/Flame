#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

namespace Flame {


	class Renderer
	{
	public:
		// TODO: δ�������Scene��������ز���������Camera��lighting, ��֤shaders�ܹ��õ���ȷ�Ļ�����ص�uniforms
		static void BeginScene();
		// TODO:
		static void EndScene();
		// TODO: ���VAOͨ��RenderCommand�µ�ָ����ݸ�RenderCommandQueue
		// Ŀǰ͵������ֱ�ӵ���RenderCommand::DrawIndexed()����
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI()
		{
			return RendererAPI::GetAPI();
		}

	};
}