#include "flamepch.h"

#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Renderer/UniformBuffer.h"
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Library/UniformBufferLibrary.h"
#include "Runtime/Library/Library.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <glm/glm.hpp>

namespace Flame
{
	Ref<Framebuffer> Renderer3D::lightFBO = nullptr;

	std::vector<std::string> m_Paths{
		"Asset/Texture/Skybox/right.jpg",
		"Asset/Texture/Skybox/left.jpg",
		"Asset/Texture/Skybox/top.jpg",
		"Asset/Texture/Skybox/bottom.jpg",
		"Asset/Texture/Skybox/front.jpg",
		"Asset/Texture/Skybox/back.jpg"
	};


	void Renderer3D::Init()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::DEPTH32F_TEX2D };
		// light depth texture uses high resolution
		fbSpec.Width = 20480;
		fbSpec.Height = 20480;
		lightFBO = Framebuffer::Create(fbSpec);
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& MeshComponent, int EntityID)
	{
		Ref<Shader> defaultShader = Library<Shader>::GetInstance().GetDefaultShader();
		defaultShader->Bind();
		if (MeshComponent.m_Mesh->bPlayAnim)
			defaultShader->SetBool("u_Animated", true);
		else
			defaultShader->SetBool("u_Animated", false);
		MeshComponent.m_Mesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().GetDefaultShader(), EntityID);
	}


	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetProjection() * glm::inverse(transform);
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetViewProjection();
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));
	}

	void Renderer3D::EndScene()
	{
	}

	
}