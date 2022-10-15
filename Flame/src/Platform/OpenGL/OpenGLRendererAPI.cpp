#include "flamepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Flame {

	void OpenGLRendererAPI::Init()
	{
		FLAME_PROFILE_FUNCTION();

		glEnable(GL_BLEND);// 这个函数用于决定, pixel绘制时, 如果已经有绘制的pixel了, 那么新pixel的权重是其alpha值, 原本的pixel的权重值是1-alpha值
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//source权重值用其alpha值，destination权重值为1-source权重值

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
		uint32_t count = indexCount ? vertexArray->GetIndexBuffer()->GetCount() : indexCount; //这句话是什么意思？
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//为什么这里nullptr不是indices(是因为index已经绑定到buffer中了吗？

		glBindTexture(GL_TEXTURE_2D, 0);
	}

}