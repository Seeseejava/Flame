#pragma once

#include "Runtime/Renderer/PostProcessing.h"

namespace Flame
{
	class OpenGLPostProcessing : public PostProcessing
	{
	public:
		OpenGLPostProcessing(const PostProcessingType& type) : PostProcessing(type) {}
	public:
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb) override;
	};
}