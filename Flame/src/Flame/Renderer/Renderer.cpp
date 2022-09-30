#include "flamepch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"


namespace Flame {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData;

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		//���û�����ص���Ϣ���������������������
		m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
		//�Ż�����ʹ����ͬ���ʵ�����ϲ���һ��Cull Frustum������λ������
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		//�ռ����������Լ���Ⱦ���������Ⱦ���������У��Ա�֮�����Ż�
		shader->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}