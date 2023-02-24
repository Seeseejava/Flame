#include "flamepch.h"

#include "Runtime/Platform/OpenGL/OpenGLPostProcessing.h"
#include "Runtime/Library/ShaderLibrary.h"

#include <glad/glad.h>

namespace Flame
{
	uint32_t OpenGLPostProcessing::DoMSAA(const Ref<Framebuffer>& fb)
	{

		uint32_t width = fb->GetSpecification().Width;
		uint32_t height = fb->GetSpecification().Height;
		
		m_Framebuffer->Bind();

		fb->BindReadFramebuffer();
		m_Framebuffer->BindDrawFramebuffer();

		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		return m_Framebuffer->GetColorAttachmentRendererID();
	}

	uint32_t OpenGLPostProcessing::DoPostWithShader(const Ref<Framebuffer>& fb, const Ref<Shader>& shader)
	{
		uint32_t width = fb->GetSpecification().Width;
		uint32_t height = fb->GetSpecification().Height;
		m_Framebuffer->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachmentRendererID());

		shader->Bind();
		shader->SetInt("screenTexture", 0);

		DoPostProcessing();

		return m_Framebuffer->GetColorAttachmentRendererID();
	}

	uint32_t OpenGLPostProcessing::ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb)
	{
		uint32_t re = 0;

		switch (m_Type)
		{
		case Flame::PostProcessingType::None:
			return 0;
			break;
		case Flame::PostProcessingType::MSAA:
			re = DoMSAA(fb);
			break;
		case Flame::PostProcessingType::Outline:
			re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_Outline"));
			break;
		case PostProcessingType::Cartoon:
			re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_Cartoon"));
			break;
		case PostProcessingType::GrayScale:
			re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_GrayScale"));
			break;
		case PostProcessingType::GaussianBlur:
			re = DoPostWithShader(fb, Library<Shader>::GetInstance().Get("Post_GaussianBlur"));
		default:
			return 0;
			break;
		}
		// it seems imgui need it ??
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return re;
	}
}