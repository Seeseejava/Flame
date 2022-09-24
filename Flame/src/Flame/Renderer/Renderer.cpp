#include "flamepch.h"
#include "Renderer.h"


namespace Flame {

	void Renderer::BeginScene()
	{
		//���û�����ص���Ϣ���������������������
	}

	void Renderer::EndScene()
	{
		//�Ż�����ʹ����ͬ���ʵ�����ϲ���һ��Cull Frustum������λ������
	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		//�ռ����������Լ���Ⱦ���������Ⱦ���������У��Ա�֮�����Ż�
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}