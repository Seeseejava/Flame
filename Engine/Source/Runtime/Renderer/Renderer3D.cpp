#include "flamepch.h"

#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Renderer/UniformBuffer.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <glm/glm.hpp>

namespace Flame
{
	static Ref<Shader> m_Shader;

	struct Renderer3DData
	{
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer3DData s_Data;

	static Ref<CubeMapTexture> m_SkyBox;
	static Ref<Shader> m_SkyBoxShader;
	static Model m_Box;

	std::vector<std::string> m_Paths{
		"Assets/textures/Skybox/right.jpg",
		"Assets/textures/Skybox/left.jpg",
		"Assets/textures/Skybox/top.jpg",
		"Assets/textures/Skybox/bottom.jpg",
		"Assets/textures/Skybox/front.jpg",
		"Assets/textures/Skybox/back.jpg"
	};


	void Renderer3D::Init()
	{
		//Shader
		m_Shader = Shader::Create(AssetManager::GetInstance().GetFullPath("Shaders/Common.glsl"));

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 1);

		m_SkyBoxShader = Shader::Create(AssetManager::GetInstance().GetFullPath("Shaders/SkyBox.glsl"));
		m_SkyBox = CubeMapTexture::Create(m_Paths);

		m_Box = Model(AssetManager::GetInstance().GetFullPath("Assets/Models/Box.obj").string());
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, int EntityID)
	{
		MeshComponent.Mesh.Draw(transform, m_Shader, EntityID);
	}


	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));
	}

	void Renderer3D::EndScene()
	{
		m_Shader->Unbind();
	}

	Ref<CubeMapTexture> Renderer3D::GetSkyBox()
	{
		return Ref<CubeMapTexture>();
	}

	Ref<CubeMapTexture> Renderer3D::GetDefaultSkyBox()
	{
		return Ref<CubeMapTexture>();
	}
	
	void Renderer3D::DrawSkyBox(const EditorCamera& camera)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::mat4(glm::mat3(camera.GetViewMatrix()));
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData), 0);

		RenderCommand::Cull(0);

		RenderCommand::DepthFunc(DepthComp::LEQUAL);
		m_SkyBoxShader->Bind();

		m_SkyBox->Bind(1);
		m_SkyBoxShader->SetInt("SkyBox", 0);
		m_Box.Draw();

		RenderCommand::DepthFunc(DepthComp::LESS);
	}
}