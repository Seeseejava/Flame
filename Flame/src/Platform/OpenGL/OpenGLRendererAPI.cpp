#include "flamepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Flame {

	void OpenGLRendererAPI::Init()
	{
		FLAME_PROFILE_FUNCTION();

		glEnable(GL_BLEND);// ����������ھ���, pixel����ʱ, ����Ѿ��л��Ƶ�pixel��, ��ô��pixel��Ȩ������alphaֵ, ԭ����pixel��Ȩ��ֵ��1-alphaֵ
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//sourceȨ��ֵ����alphaֵ��destinationȨ��ֵΪ1-sourceȨ��ֵ

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? vertexArray->GetIndexBuffer()->GetCount() : indexCount; //��仰��ʲô��˼��
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//Ϊʲô����nullptr����indices(����Ϊindex�Ѿ��󶨵�buffer������

		glBindTexture(GL_TEXTURE_2D, 0);
	}

}