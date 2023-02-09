#pragma once

#include "Runtime/Renderer/Framebuffer.h"

namespace Flame
{
	enum class PostProcessingType
	{
		None = 1,
		MSAA,

	};

	class PostProcessing
	{
	public:
		PostProcessing(const PostProcessingType& type) : m_Type{ type } {};
		PostProcessing(const PostProcessing&) = default;
		virtual ~PostProcessing() {}
	public:
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb) { return 0; };
	protected:
		PostProcessingType m_Type;
	};
}