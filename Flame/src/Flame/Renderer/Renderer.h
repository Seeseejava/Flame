#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"
namespace Flame {


	class Renderer
	{
	public:
		// TODO: 未来会接受Scene场景的相关参数，比如Camera、lighting, 保证shaders能够得到正确的环境相关的uniforms
		static void BeginScene(OrthographicCamera& camera);//这里传引用时因为不想把camera全部矩阵都传进来，暂时不考虑传引用的坏处
		// TODO:
		static void EndScene();
		// TODO: 会把VAO通过RenderCommand下的指令，传递给RenderCommandQueue
		// 目前偷个懒，直接调用RenderCommand::DrawIndexed()函数
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

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