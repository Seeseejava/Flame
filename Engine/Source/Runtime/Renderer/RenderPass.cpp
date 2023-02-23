#include "flamepch.h"

#include "Runtime/Renderer/RenderPass.h"
#include "Runtime/Renderer/Renderer.h"
#include "Runtime/Platform/OpenGL/OpenGLPostProcessing.h"

namespace Flame
{
	void RenderPass::AddPostProcessing(PostProcessingType type)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    FLAME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return;
		case RendererAPI::API::OpenGL:  m_PostProcessings.emplace_back(std::make_unique<OpenGLPostProcessing>(type)); return;
		}

		FLAME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return;
	}

	uint32_t RenderPass::ExcuteAndReturnFinalTex()
	{
		uint32_t width = m_Specification.TargetFramebuffer->GetSpecification().Width;
		uint32_t height = m_Specification.TargetFramebuffer->GetSpecification().Height;
		PostProcessing::m_Framebuffer->Bind();
		PostProcessing::m_Framebuffer->Resize(width, height);

		uint32_t final = 0;
		for (auto& effect : m_PostProcessings)
		{
			final = effect->ExcuteAndReturnFinalTex(m_Specification.TargetFramebuffer);
		}
		return final;
	}
}