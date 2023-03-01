#pragma once

#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Texture.h"

namespace Flame
{
	enum class PostProcessingType
	{
		None = 0,
		MSAA = 1,
		Outline,
		Cartoon,
		GrayScale,
		GaussianBlur,
		FxaaConsole,
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
		PostProcessingType m_Type;
		static Ref<Framebuffer> m_Framebuffer;
		static Ref<Texture2D>   m_IntermediateScreenTex;
	protected:
		static Ref<VertexArray>  m_ScreenQuadVAO;
		static Ref<VertexBuffer> m_ScreenQuadVBO;
		static Ref<IndexBuffer>  m_ScreenQuadIBO;
	};
}