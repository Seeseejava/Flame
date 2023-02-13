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

#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <glm/glm.hpp>

namespace Flame
{
	static Ref<Shader> m_Shader;

	static Ref<CubeMapTexture> m_SkyBox;
	static Ref<Shader> m_SkyBoxShader;
	static Model m_Box;

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
		//Shader

		m_SkyBoxShader = Shader::Create(AssetManager::GetFullPath("Shaders/SkyBox.glsl"));
		m_SkyBox = CubeMapTexture::Create(m_Paths);

		m_Box = Model(AssetManager::GetFullPath("Assets/Models/Box.obj").string());
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, StaticMeshComponent& MeshComponent, int EntityID)
	{
		MeshComponent.Mesh.Draw(transform, cameraPos, EntityID);
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

	Ref<CubeMapTexture> Renderer3D::GetSkyBox()
	{
		return m_SkyBox;
	}

	Ref<CubeMapTexture> Renderer3D::GetDefaultSkyBox()
	{
		m_SkyBox = CubeMapTexture::Create(m_Paths);
		return m_SkyBox;
	}
	
	void Renderer3D::DrawSkyBox(const EditorCamera& camera)
	{
		Ref<UniformBuffer> cameraUniform = Library<UniformBuffer>::GetInstance().GetCameraUniformBuffer();
		glm::mat4 ViewProjection = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		cameraUniform->SetData(&ViewProjection, sizeof(ViewProjection));

		RenderCommand::Cull(0);

		RenderCommand::DepthFunc(DepthComp::LEQUAL);
		m_SkyBoxShader->Bind();

		m_SkyBox->Bind(0);
		m_SkyBoxShader->SetInt("SkyBox", 0);
		Library<Model>::GetInstance().Get("Box")->Draw();

		RenderCommand::DepthFunc(DepthComp::LESS);
	}
}