#include "flamepch.h"

#include "Runtime/Renderer/StaticMesh.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/TextureLibrary.h"
#include "Runtime/Renderer/Model.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

#include <Glad/glad.h>

namespace Flame {

	StaticMesh::StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices)
		: m_Vertices(vertices), m_Indices(indices)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(Vertex) * vertices.size());
		m_VertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Int,	  "a_EntityID"},
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(indices.size());

		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	StaticMesh::StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(Vertex) * vertices.size());
		m_VertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Int,	  "a_EntityID"},
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(indices.size());

		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void StaticMesh::Draw(const glm::mat4& transform, const glm::vec3& cameraPos, const Ref<Shader>& shader, int entityID, Model* model)
	{
		SetupMesh(entityID);
		if (ModeManager::bHdrUse)
		{
			shader->Bind();
			shader->SetMat4("model", transform);
			shader->SetFloat3("camPos", cameraPos);

			Library<CubeMapTexture>::GetInstance().Get("EnvironmentIrradiance")->Bind(0);
			Library<CubeMapTexture>::GetInstance().Get("EnvironmentPrefilter")->Bind(1);
			Library<Texture2D>::GetInstance().Get("BRDF_LUT")->Bind(2);

			model->m_AlbedoMap->Bind(3);
			model->m_NormalMap->Bind(4);
			model->m_MetallicMap->Bind(5);
			model->m_RoughnessMap->Bind(6);
			model->m_AoMap->Bind(7);

			shader->SetInt("irradianceMap", 0);
			shader->SetInt("prefilterMap", 1);
			shader->SetInt("brdfLUT", 2);
			shader->SetInt("albedoMap", 3);
			shader->SetInt("normalMap", 4);
			shader->SetInt("metallicMap", 5);
			shader->SetInt("roughnessMap", 6);
			shader->SetInt("aoMap", 7);
		}
		else
		{
			shader->Bind();
			shader->SetMat4("u_Model.Transform", (transform));
			m_VertexArray->Bind();

			model->m_AlbedoMap->Bind(0);
			model->m_NormalMap->Bind(1);
			model->m_MetallicMap->Bind(2);
			model->m_RoughnessMap->Bind(3);
			model->m_AoMap->Bind(4);

			shader->SetInt("albedoMap", 0);
			shader->SetInt("normalMap", 1);
			shader->SetInt("metallicMap", 2);
			shader->SetInt("roughnessMap", 3);
			shader->SetInt("aoMap", 4);
			shader->SetFloat3("u_Uniform.camPos", cameraPos);
		}

		RenderCommand::DrawIndexed(m_VertexArray, m_IndexBuffer->GetCount());
	}

	void StaticMesh::Draw()
	{
		//SetupMesh(EntityID);
		static bool bInit = true;
		if (bInit)
		{
			bInit = false;

			m_VertexArray->Bind();

			m_VertexBuffer->SetData(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size());
			m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size());

			m_VertexArray->Unbind();
		}
		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray, m_IndexBuffer->GetCount());
		m_VertexArray->Unbind();
	}

	void StaticMesh::SetupMesh(int entityID)
	{
		if (m_EntityID == -1)
		{
			m_EntityID = entityID;
			m_VertexArray->Bind();

			for (int i = 0; i < m_Vertices.size(); ++i)
			{
				m_Vertices[i].EntityID = entityID;
			}

			m_VertexBuffer->SetData(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size());

			m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size());

			m_VertexArray->Unbind();
		}
	}
}