#include "flamepch.h"
#include "Renderer.h"
#include "Runtime/Renderer/Renderer2D.h"
#include "Runtime/Renderer/Renderer3D.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Runtime/Renderer/PostProcessing.h"


namespace Flame {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		FLAME_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();
		PostProcessing::Init();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}


	void Renderer::EndScene()
	{
		//优化：将使用相同材质的物体合并到一起、Cull Frustum、根据位置排序
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		//收集场景数据以及渲染命令，并将渲染命令加入队列，以便之后作优化
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}