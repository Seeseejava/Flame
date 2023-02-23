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
		m_Framebuffer->Resize(width, height);

		fb->BindReadFramebuffer();
		m_Framebuffer->BindDrawFramebuffer();

		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		return m_Framebuffer->GetColorAttachmentRendererID();
	}

	uint32_t OpenGLPostProcessing::DoOutline(const Ref<Framebuffer>& fb)
	{
		Library<Shader>::GetInstance().Get("Post_Outline")->Bind();
		Library<Shader>::GetInstance().Get("Post_Outline")->SetInt("screenTexture", 0);
		fb->Bind();
		DoPostProcessing();
		return fb->GetColorAttachmentRendererID();
	}

	uint32_t OpenGLPostProcessing::ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb)
	{
		switch (m_Type)
		{
		case Flame::PostProcessingType::None:
			return 0;
			break;
		case Flame::PostProcessingType::MSAA:
			return DoMSAA(fb);
			break;
		case Flame::PostProcessingType::Outline:
			return DoOutline(fb);
			break;
		default:
			return 0;
			break;
		}

		return 0;
	}
}