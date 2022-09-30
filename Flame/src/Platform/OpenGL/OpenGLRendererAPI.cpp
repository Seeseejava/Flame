#include "flamepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Flame {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);// 这个函数用于决定, pixel绘制时, 如果已经有绘制的pixel了, 那么新pixel的权重是其alpha值, 原本的pixel的权重值是1-alpha值
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//source权重值用其alpha值，destination权重值为1-source权重值
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		//为什么这里nullptr不是indices(是因为index已经绑定到buffer中了吗？
	}

}