#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"

#include "OrthographicCamera.h"
#include "Shader.h"
namespace Flame {


	class Renderer
	{
	public:
		// TODO: δ�������Scene��������ز���������Camera��lighting, ��֤shaders�ܹ��õ���ȷ�Ļ�����ص�uniforms
		static void BeginScene(OrthographicCamera& camera);//���ﴫ����ʱ��Ϊ�����cameraȫ�����󶼴���������ʱ�����Ǵ����õĻ���
		// TODO:
		static void EndScene();
		// TODO: ���VAOͨ��RenderCommand�µ�ָ����ݸ�RenderCommandQueue
		// Ŀǰ͵������ֱ�ӵ���RenderCommand::DrawIndexed()����
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