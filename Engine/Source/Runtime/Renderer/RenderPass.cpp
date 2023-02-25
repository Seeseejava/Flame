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
		PostProcessing::m_IntermediateScreenTex = Texture2D::Create(
			m_Specification.TargetFramebuffer->GetSpecification().Width,
			m_Specification.TargetFramebuffer->GetSpecification().Height
		);

		uint32_t final = 0;
		for (auto& effect : m_PostProcessings)
		{
			final = effect->ExcuteAndReturnFinalTex(m_Specification.TargetFramebuffer);
		}
		return final;
	}
}