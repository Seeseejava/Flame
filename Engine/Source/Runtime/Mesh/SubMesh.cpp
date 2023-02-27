#include "flamepch.h"

#include "Runtime/Mesh/Mesh.h"
#include "Runtime/Renderer/RenderCommand.h"
#include "Runtime/Library/TextureLibrary.h"
#include "Runtime/Mesh/SubMesh.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

#include <Glad/glad.h>

namespace Flame {

	SubMesh::SubMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices)
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

	SubMesh::SubMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures, uint32_t materialIndex)
		: m_StaticVertices(vertices), m_Indices(indices), m_Textures(textures), m_MaterialIndex(materialIndex)
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

	SubMesh::SubMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices)
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

	SubMesh::SubMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures, uint32_t materialIndex)
		: m_SkinnedVertices(vertices), m_Indices(indices), m_Textures(textures), m_MaterialIndex(materialIndex)
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

	void SubMesh::Draw(const glm::mat4& transform, const glm::vec3& cameraPos, const Ref<Shader>& shader, int entityID, Mesh* model)
	{
		SetupMesh(entityID);

		shader->Bind();
		if (model->bPlayAnim)
		{
			if (!model->bStopAnim)
				model->m_Animator.UpdateAnimation(0.01f * model->m_AnimPlaySpeed);

			auto transforms = model->m_Animator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		}

		shader->SetMat4("model", transform);
		shader->SetFloat3("camPos", cameraPos);

		if (ModeManager::bHdrUse)
		{

			Library<CubeMapTexture>::GetInstance().Get("EnvironmentIrradiance")->Bind(0);
			Library<CubeMapTexture>::GetInstance().Get("EnvironmentPrefilter")->Bind(1);
			Library<Texture2D>::GetInstance().Get("BRDF_LUT")->Bind(2);

		}
		else
		{
			Library<CubeMapTexture>::GetInstance().Get("BlackCubeMap")->Bind(0);
			Library<CubeMapTexture>::GetInstance().Get("BlackCubeMap")->Bind(1);
			Library<Texture2D>::GetInstance().Get("BlackTexture")->Bind(2);
		}
			if (model->m_Material[m_MaterialIndex]->bUseAlbedoMap)
				model->m_Material[m_MaterialIndex]->m_AlbedoMap->Bind(3);
			else
				model->m_Material[m_MaterialIndex]->albedoRGBA->Bind(3);

			if (model->m_Material[m_MaterialIndex]->bUseNormalMap)
				model->m_Material[m_MaterialIndex]->m_NormalMap->Bind(4);
			else
				Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(4);

			if (model->m_Material[m_MaterialIndex]->bUseMetallicMap)
				model->m_Material[m_MaterialIndex]->m_MetallicMap->Bind(5);
			else
				model->m_Material[m_MaterialIndex]->metallicRGBA->Bind(5);

			if (model->m_Material[m_MaterialIndex]->bUseRoughnessMap)
				model->m_Material[m_MaterialIndex]->m_RoughnessMap->Bind(6);
			else
				model->m_Material[m_MaterialIndex]->roughnessRGBA->Bind(6);

			if (model->m_Material[m_MaterialIndex]->bUseAoMap)
				model->m_Material[m_MaterialIndex]->m_AoMap->Bind(7);
			else
				Library<Texture2D>::GetInstance().GetWhiteTexture()->Bind(7);

		shader->SetInt("irradianceMap", 0);
		shader->SetInt("prefilterMap", 1);
		shader->SetInt("brdfLUT", 2);
		shader->SetInt("albedoMap", 3);
		shader->SetInt("normalMap", 4);
		shader->SetInt("metallicMap", 5);
		shader->SetInt("roughnessMap", 6);
		shader->SetInt("aoMap", 7);

		RenderCommand::DrawIndexed(m_VertexArray, m_IndexBuffer->GetCount());
	}

	void SubMesh::Draw()
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

	void SubMesh::SetupMesh(int entityID)
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