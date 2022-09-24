#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

namespace Flame {


	class Renderer
	{
	public:
		// TODO: 未来会接受Scene场景的相关参数，比如Camera、lighting, 保证shaders能够得到正确的环境相关的uniforms
		static void BeginScene();
		// TODO:
		static void EndScene();
		// TODO: 会把VAO通过RenderCommand下的指令，传递给RenderCommandQueue
		// 目前偷个懒，直接调用RenderCommand::DrawIndexed()函数
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI()
		{
			return RendererAPI::GetAPI();
		}

	};
}