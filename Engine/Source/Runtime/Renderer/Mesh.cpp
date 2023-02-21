#include "flamepch.h"

#include "Runtime/Renderer/Mesh.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/TextureLibrary.h"
#include "Runtime/Renderer/Model.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

#include <Glad/glad.h>

namespace Flame {

	Mesh::Mesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices)
		: m_StaticVertices(vertices), m_Indices(indices)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(StaticVertex) * vertices.size());
		m_VertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(indices.size());

		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	Mesh::Mesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures)
		: m_StaticVertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(StaticVertex) * vertices.size());
		m_VertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(indices.size());

		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	Mesh::Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices)
		: m_SkinnedVertices(vertices), m_Indices(indices)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(SkinnedVertex) * vertices.size());
		m_VertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
					{ ShaderDataType::Int4,   "a_BoneIDs"},
					{ ShaderDataType::Float4, "a_Weights"},
			});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer = IndexBuffer::Create(indices.size());

		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	Mesh::Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures)
		: m_SkinnedVertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(SkinnedVertex) * vertices.size());
		m_VertexBuffer->SetLayout({
					{ ShaderDataType::Float3, "a_Pos"},
					{ ShaderDataType::Float3, "a_Normal"},
					{ ShaderDataType::Float2, "a_TexCoord"},
					{ ShaderDataType::Float3, "a_Tangent"},
					{ ShaderDataType::Float3, "a_Bitangent"},
					{ ShaderDataType::Int,	  "a_EntityID"},
					{ ShaderDataType::Int4,   "a_BoneIDs"},
					{ ShaderDataType::Float4, "a_Weights"},
			});
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_IndexBuffer = IndexBuffer::Create(indices.size());
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
	}

	void Mesh::Draw(const glm::mat4& transform, const glm::vec3& cameraPos, const Ref<Shader>& shader, int entityID, Model* model)
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

			if (model->bUseAlbedoMap)
				model->m_AlbedoMap->Bind(3);
			else
				model->albedoRGBA->Bind(3);

			if (model->bUseNormalMap)
				model->m_NormalMap->Bind(4);
			else
				Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(4);

			if (model->bUseMetallicMap)
				model->m_MetallicMap->Bind(5);
			else
				model->metallicRGBA->Bind(5);

			if (model->bUseRoughnessMap)
				model->m_RoughnessMap->Bind(6);
			else
				model->roughnessRGBA->Bind(6);

			if (model->bUseAoMap)
				model->m_AoMap->Bind(7);
			else
				Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(7);

			if (model->bAnimated)
			{
				model->m_Animator.UpdateAnimation(0.01f);

				auto transforms = model->m_Animator.GetFinalBoneMatrices();
				for (int i = 0; i < transforms.size(); ++i)
					shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			}

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
			shader->SetMat4("u_Model_transform", (transform));
			m_VertexArray->Bind();
			if (model->bUseAlbedoMap)
				model->m_AlbedoMap->Bind(0);
			else
				model->albedoRGBA->Bind(0);

			if (model->bUseNormalMap)
				model->m_NormalMap->Bind(1);
			else
				Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(1);

			if (model->bUseMetallicMap)
				model->m_MetallicMap->Bind(2);
			else
				model->metallicRGBA->Bind(2);

			if (model->bUseRoughnessMap)
				model->m_RoughnessMap->Bind(3);
			else
				model->roughnessRGBA->Bind(3);

			if (model->bUseAoMap)
				model->m_AoMap->Bind(4);
			else
				Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(4);

			shader->SetInt("albedoMap", 0);
			shader->SetInt("normalMap", 1);
			shader->SetInt("metallicMap", 2);
			shader->SetInt("roughnessMap", 3);
			shader->SetInt("aoMap", 4);
			shader->SetFloat3("u_Uniform.camPos", cameraPos);
		}

		RenderCommand::DrawIndexed(m_VertexArray, m_IndexBuffer->GetCount());
	}

	void Mesh::Draw()
	{
		//SetupMesh(EntityID);
		static bool bInit = true;
		if (bInit)
		{
			bInit = false;

			m_VertexArray->Bind();

			if (m_StaticVertices.empty())
				m_VertexBuffer->SetData(m_SkinnedVertices.data(), sizeof(StaticVertex) * m_SkinnedVertices.size());
			else
				m_VertexBuffer->SetData(m_StaticVertices.data(), sizeof(StaticVertex) * m_StaticVertices.size());

			m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size());

			m_VertexArray->Unbind();
		}
		m_VertexArray->Bind();
		RenderCommand::DrawIndexed(m_VertexArray, m_IndexBuffer->GetCount());
		m_VertexArray->Unbind();
	}

	void Mesh::SetupMesh(int entityID)
	{
		if (m_EntityID == -1)
		{
			m_EntityID = entityID;
			m_VertexArray->Bind();

			if (m_StaticVertices.empty())
			{
				for (int i = 0; i < m_SkinnedVertices.size(); ++i)
				{
					m_SkinnedVertices[i].EntityID = entityID;
				}

				m_VertexBuffer->SetData(m_SkinnedVertices.data(), sizeof(SkinnedVertex) * m_SkinnedVertices.size());
			}
			else
			{
				for (int i = 0; i < m_StaticVertices.size(); ++i)
				{
					m_StaticVertices[i].EntityID = entityID;
				}

				m_VertexBuffer->SetData(m_StaticVertices.data(), sizeof(StaticVertex) * m_StaticVertices.size());
			}
			m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size());

			m_VertexArray->Unbind();
		}
	}
}