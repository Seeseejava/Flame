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

	};

	class PostProcessing
	{
		friend class RenderPass;
	public:
		PostProcessing(const PostProcessingType& type);
		virtual ~PostProcessing() {}
	public:
		static void Init();
		virtual uint32_t ExcuteAndReturnFinalTex(const Ref<Framebuffer>& fb) { return 0; };
	protected:
		void DoPostProcessing();
	protected:
		PostProcessingType m_Type;
		static Ref<VertexArray> m_ScreenQuadVAO;
		static Ref<VertexBuffer> m_ScreenQuadVBO;
		static Ref<IndexBuffer> m_ScreenQuadIBO;
		static Ref<Framebuffer> m_Framebuffer;
	};
}