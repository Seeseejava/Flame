#include "flamepch.h"

#include "Runtime/Renderer/StaticMesh.h"
#include "Runtime/Renderer/RenderCommand.h"

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

	void StaticMesh::Draw(const glm::mat4& transform, const Ref<Shader>& shader, int entityID)
	{
		SetupMesh(entityID);
		shader->Bind();
		shader->SetMat4("u_Model", (transform));
		m_VertexArray->Bind();
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