#pragma once

#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Texture.h"

namespace Flame
{
	enum class PostProcessingType
	{
		None = 1,
		MSAA,
		Outline,
		Cartoon,
		GrayScale,
		GaussianBlur,

	};

	class PostProcessing
	{
	public:
		PostProcessing(const PostProcessingType& type);
		virtual ~PostProcessing() {}
	public:
		static void Init();
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb) { return 0; };
	protected:
		void DoPostProcessing();
	public:
		static Ref<Framebuffer> m_Framebuffer;
	protected:
		PostProcessingType m_Type;
		static Ref<VertexArray> m_ScreenQuadVAO;
		static Ref<VertexBuffer> m_ScreenQuadVBO;
		static Ref<IndexBuffer> m_ScreenQuadIBO;
	};
}