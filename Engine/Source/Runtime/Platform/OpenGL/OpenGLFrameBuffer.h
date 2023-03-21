#pragma once

#include "Runtime/Renderer/Framebuffer.h"

namespace Flame {

	class OpenGLFramebuffer : public Framebuffer
	{
		friend class OpenGLPostProcessing;
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void BindReadFramebuffer() override;
		virtual void BindDrawFramebuffer() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { FLAME_CORE_ASSERT(index < m_ColorAttachments.size(), "Index Error!"); return m_ColorAttachments[index];}
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

		virtual void FramebufferTexture2D(uint32_t cubemapIndex, uint32_t cubemapID, uint32_t slot = 0) override;

		virtual Ref<class Texture3D> GetDepthTex3D() const override; // have bugs
		virtual void BindDepthTex2D(uint32_t slot) override;
		virtual void UnBindDepthTex2D(uint32_t slot) override;
	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}