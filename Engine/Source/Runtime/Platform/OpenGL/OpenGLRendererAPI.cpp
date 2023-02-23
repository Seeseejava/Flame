#include "flamepch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Flame {

	//prepare for skybox
	namespace Utils
	{
		static GLenum StencilFuncToOpenGLStencilFunc(StencilFunc func)
		{
			switch (func)
			{
			case StencilFunc::ALWAYS:
				return GL_ALWAYS;
			case StencilFunc::NOTEQUAL:
				return GL_NOTEQUAL;
			}
			FLAME_CORE_ASSERT(false, "Unknow StencilFunc");
			return -1;
		}

		static GLenum StencilOpToOpenGLStencilOp(StencilOp op)
		{
			switch (op)
			{
			case StencilOp::KEEP:
				return GL_KEEP;
			case StencilOp::INCREMENT:
				return GL_INCR_WRAP;
			case StencilOp::DECREMENT:
				return GL_DECR_WRAP;
			}
			FLAME_CORE_ASSERT(false, "Unknown StencilOp");
			return -1;
		}

		static GLenum DepthcomparisonToOpenGLDepthcomparison(DepthComp comp)
		{
			switch (comp)
			{
			case DepthComp::EQUAL:
				return GL_EQUAL;
			case DepthComp::LEQUAL:
				return GL_LEQUAL;
			case DepthComp::LESS:
				return GL_LESS;
			}
			FLAME_CORE_ASSERT(false, "Unknown DepthComp");
			return -1;
		}
	}

	void OpenGLRendererAPI::Init()
	{
		FLAME_PROFILE_FUNCTION();

		glEnable(GL_BLEND);// 这个函数用于决定, pixel绘制时, 如果已经有绘制的pixel了, 那么新pixel的权重是其alpha值, 原本的pixel的权重值是1-alpha值
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//source权重值用其alpha值，destination权重值为1-source权重值

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0x00); // forbidden to write in stencil

		glEnable(GL_LINE_SMOOTH); //线条平滑，相当于抗锯齿
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
		vertexArray->Bind(); // 没有这个就没有渲染数据了
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount(); //这句话是什么意思？
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//为什么这里nullptr不是indices(是因为index已经绑定到buffer中了吗？

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void OpenGLRendererAPI::ClearStencil()
	{
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DepthMask(bool maskFlag)
	{
		if (maskFlag) glDepthMask(GL_TRUE);
		else glDepthMask(GL_FALSE);
	}

	void OpenGLRendererAPI::DepthTest(int32_t Bit)
	{
		if (Bit) glEnable(GL_DEPTH_TEST);
		else glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::Blend(int32_t Bit)
	{
		if (Bit)
		{
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void OpenGLRendererAPI::StencilTest(int32_t Bit)
	{
		if (Bit) glEnable(GL_STENCIL_TEST);
		else glDisable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::Cull(int32_t Bit)
	{
		if (Bit) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::CullFrontOrBack(int32_t Bit)
	{
		if (Bit) glCullFace(GL_FRONT);
		else glCullFace(GL_BACK);
	}

	void OpenGLRendererAPI::SetStencilFunc(StencilFunc stencilFunc, int32_t ref, int32_t mask)
	{
		glStencilFunc(Utils::StencilFuncToOpenGLStencilFunc(stencilFunc), ref, mask);
	}

	void OpenGLRendererAPI::StencilMask(uint32_t mask)
	{
		// glStencilMask(0x00): forbidden to write in stencil
		// glStencilMask(0xFF): allow to write in stencil
		glStencilMask(mask);
	}

	void OpenGLRendererAPI::SetFrontOrBackStencilOp(int32_t FrontOrBack, StencilOp stencilFail, StencilOp depthFail, StencilOp depthSuccess)
	{
		//Front
		if (FrontOrBack)
		{
			glStencilOpSeparate(GL_FRONT, Utils::StencilOpToOpenGLStencilOp(stencilFail), Utils::StencilOpToOpenGLStencilOp(depthFail), Utils::StencilOpToOpenGLStencilOp(depthSuccess));
		}
		else
		{
			glStencilOpSeparate(GL_BACK, Utils::StencilOpToOpenGLStencilOp(stencilFail), Utils::StencilOpToOpenGLStencilOp(depthFail), Utils::StencilOpToOpenGLStencilOp(depthSuccess));
		}
	}

	void OpenGLRendererAPI::DepthFunc(DepthComp comp)
	{
		glDepthFunc(Utils::DepthcomparisonToOpenGLDepthcomparison(comp));
	}

	void OpenGLRendererAPI::BindTexture(int32_t slot, uint32_t textureID)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	int OpenGLRendererAPI::GetDrawFrameBuffer()
	{
		int framebufferID = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &framebufferID);
		return framebufferID;
	}

	void OpenGLRendererAPI::BindFrameBuffer(uint32_t framebufferID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	}
}