#include "flamepch.h"

#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Renderer/UniformBuffer.h"
#include "Runtime/Library/ShaderLibrary.h"
#include "Runtime/Library/UniformBufferLibrary.h"
#include "Runtime/Library/Library.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <glm/glm.hpp>

namespace Flame
{

	std::vector<std::string> m_Paths{
		"Assets/texture/Skybox/right.jpg",
		"Assets/texture/Skybox/left.jpg",
		"Assets/texture/Skybox/top.jpg",
		"Assets/texture/Skybox/bottom.jpg",
		"Assets/texture/Skybox/front.jpg",
		"Assets/texture/Skybox/back.jpg"
	};


	void Renderer3D::Init()
	{

	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& MeshComponent, int EntityID)
	{
		if (ModeManager::bHdrUse)
			MeshComponent.m_Mesh->Draw(transform, cameraPos, Library<Shader>::GetInstance().Get("IBL_pbr"), EntityID);
		else
			MeshComponent.m_Mesh->Draw(transform, cameraPos, EntityID);
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