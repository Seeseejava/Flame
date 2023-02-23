#include "flamepch.h"

#include "Runtime/Renderer/PostProcessing.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

namespace Flame
{
	Ref<VertexArray> PostProcessing::m_ScreenQuadVAO = nullptr;
	Ref<VertexBuffer> PostProcessing::m_ScreenQuadVBO = nullptr;
	Ref<IndexBuffer> PostProcessing::m_ScreenQuadIBO = nullptr;
	Ref<Framebuffer> PostProcessing::m_Framebuffer = nullptr;

	PostProcessing::PostProcessing(const PostProcessingType& type)
		: m_Type{ type }
	{
	}

	void PostProcessing::Init()
	{
		m_ScreenQuadVAO = VertexArray::Create();
		float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		uint32_t screenQuadIndices[] = {
			0, 1, 2, 0, 2, 3
		};
		m_ScreenQuadIBO = IndexBuffer::Create(screenQuadIndices, 6);
		m_ScreenQuadVBO = VertexBuffer::Create(screenQuadVertices, sizeof(screenQuadVertices));
		m_ScreenQuadVBO->SetLayout({
			{ ShaderDataType::Float2,	"a_Position" },
			{ ShaderDataType::Float2,	"a_TexCoord" },
			});
		m_ScreenQuadVAO->AddVertexBuffer(m_ScreenQuadVBO);
		m_ScreenQuadVAO->SetIndexBuffer(m_ScreenQuadIBO);

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
		fbSpec.Width = ConfigManager::m_ViewportSize.x;
		fbSpec.Height = ConfigManager::m_ViewportSize.y;
		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void PostProcessing::DoPostProcessing()
	{
		RenderCommand::DrawIndexed(m_ScreenQuadVAO);
	}
}