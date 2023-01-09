#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

#include "Shader.h"
namespace Flame {


	class Renderer
	{
	public:
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void EndScene();
		// TODO: 会把VAO通过RenderCommand下的指令，传递给RenderCommandQueue
		// 目前偷个懒，直接调用RenderCommand::DrawIndexed()函数
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI()
		{
			return RendererAPI::GetAPI();
		}

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};
}