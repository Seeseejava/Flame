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
}