#pragma once

#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/PostProcessing.h"

namespace Flame
{
	struct RenderPassSpecification
	{
		Ref<Framebuffer> TargetFramebuffer = nullptr;
		std::string DebugName = "";
	};

	class RenderPass
	{
	public:
		RenderPass(RenderPassSpecification Spec) : m_Specification(Spec) {};
		virtual ~RenderPass() = default;

	public:
		RenderPassSpecification& GetSpecification() { return m_Specification; };
		const RenderPassSpecification& GetSpecification() const { return m_Specification; };

		void AddPostProcessing(PostProcessingType type);
		uint32_t ExcuteAndReturnFinalTex();
	private:
		RenderPassSpecification m_Specification;
		std::vector<std::unique_ptr<PostProcessing>> m_PostProcessings;
	};
}