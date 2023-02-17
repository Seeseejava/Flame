#include "flamepch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Flame {

	namespace Utils {

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void AttachColorTexture(uint32_t& id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) //id 为什么改为引用(不改会报错）
		{
			glGenTextures(1, &id);
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_TRUE);
				// internalFormat：指定OpenGL是如何管理纹理单元中数据格式的
				// format：指定data所指向的数据的格式
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, id);
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachColorRenderBuffer(uint32_t id, int samples, GLenum internalFormat, uint32_t width, uint32_t height, int index)
		{
			glGenRenderbuffers(1, &id);
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, id);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}
			else
			{
				glBindRenderbuffer(GL_RENDERBUFFER, id);
				glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, id);
		}

		static void AttachDepthTexture(uint32_t& id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			glGenTextures(1, &id);
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, id);
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glBindTexture(GL_TEXTURE_2D, id);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}


		static void AttachDepthRenderBuffer(uint32_t& id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{

			glGenRenderbuffers(1, &id);
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, id);
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}
			else
			{
				glBindRenderbuffer(GL_RENDERBUFFER, id);
				glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
			}
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, id);
		}


		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static GLenum FlameFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
			case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			FLAME_CORE_ASSERT(false, "No such kind of format!");
			return 0;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data()); //C++11 返回内置vector所指的数组内存的第一个元素的指针
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

#if OLD_PATH
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//attach it to the frame buffer, 作为输出的texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
		// 	GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
#endif
		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						//Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
						Utils::AttachColorRenderBuffer(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, m_Specification.Width, m_Specification.Height, i); // dont use multisample in Renderbuffer
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH:
					Utils::AttachDepthRenderBuffer(m_DepthAttachment, m_Specification.Samples, GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthRenderBuffer(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			FLAME_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Size bigger than 4");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
			//glDrawBuffers函数并不是一个Draw Call命令，而是一个状态机参数设置的函数，
			//它的作用是告诉OpenGL，把绘制output填充到这些Attachment对应的Buffer里，所以这个函数在创建Framebuffer的时候就可以被调用了
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		FLAME_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		//glViewport(0, 0, m_Specification.Width, m_Specification.Height); //glViewport会显示在imgui的窗口上，从左下角开始算起
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::BindReadFramebuffer()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::BindDrawFramebuffer()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		FLAME_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Index Error");

		// TODO: Remove the intermediate code
		// First, Copy the framebuffer to the intermediateFBO (must do this if multisample)

		uint32_t width = m_Specification.Width;
		uint32_t height = m_Specification.Height;
		//unsigned int intermediateFBO;
		//glGenFramebuffers(1, &intermediateFBO);

		static bool bInit = true;
		static unsigned int intermediateFBO;
		static unsigned int tempTex;

		if (bInit)
		{
			glGenFramebuffers(1, &intermediateFBO);
			glGenRenderbuffers(1, &tempTex); // 为什么要这样做
			bInit = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

		//unsigned int tempTex;
		//glGenRenderbuffers(1, &tempTex);
		glBindRenderbuffer(GL_RENDERBUFFER, tempTex);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_R32I, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, tempTex);

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glNamedFramebufferReadBuffer(m_RendererID, GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glNamedFramebufferDrawBuffer(intermediateFBO, GL_COLOR_ATTACHMENT0 + attachmentIndex);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, intermediateFBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		return pixelData;

	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		FLAME_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Index Error");

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		switch (spec.TextureFormat)
		{
			case FramebufferTextureFormat::RED_INTEGER:
				glClearBufferiv(GL_COLOR, attachmentIndex, &value);
				//glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value);
				break;
			case FramebufferTextureFormat::RGBA8:
				glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RGBA8, GL_INT, &value);
				break;
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				glClearBufferiv(GL_DEPTH24_STENCIL8, attachmentIndex, &value);
				break;
			case FramebufferTextureFormat::DEPTH:
				glClearBufferiv(GL_DEPTH_COMPONENT24, attachmentIndex, &value);
				break;
		}
	}


	void OpenGLFramebuffer::FramebufferTexture2D(uint32_t cubemapIndex, uint32_t cubemapID, uint32_t slot)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubemapIndex, cubemapID, 0);
	}
}