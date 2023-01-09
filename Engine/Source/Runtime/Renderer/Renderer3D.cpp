#include "flamepch.h"

#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/RenderCommand.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"

#include <glm/glm.hpp>

namespace Flame
{
	struct CubeVertex
	{
		glm::vec3 Pos;
		//glm::vec4 Color;
		glm::vec2 Coord;
		glm::vec3 Normal;
	};

	struct Renderer3DData
	{
		static const int MaxTriangles = 30000;
		static const int MaxVertices = MaxTriangles * 3;
		static const int MaxIndices = MaxVertices * 1000;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Shader> m_Shader;
	};

	static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
		s_Data.m_VertexArray = VertexArray::Create();

		s_Data.m_VertexBuffer = VertexBuffer::Create(Renderer3DData::MaxVertices * sizeof(CubeVertex));

		s_Data.m_VertexBuffer->SetLayout({
			{ShaderDataType::Float3, "a_Pos"},
			//{ShaderDataType::Float3, "a_Color"},
			{ShaderDataType::Float2, "a_Coord"},
			{ShaderDataType::Float3, "a_Normal"},
			}
		);
		s_Data.m_VertexArray->AddVertexBuffer(s_Data.m_VertexBuffer);

		int IndicesOffset = 0;
		uint32_t* CubeIndices = new uint32_t[12];
		for (int i = 0; i < 2; ++i)
		{
			CubeIndices[i * 6] = IndicesOffset;
			CubeIndices[i * 6 + 1] = IndicesOffset + 1;
			CubeIndices[i * 6 + 2] = IndicesOffset + 2;
			CubeIndices[i * 6 + 3] = IndicesOffset + 3;
			CubeIndices[i * 6 + 4] = IndicesOffset + 4;
			CubeIndices[i * 6 + 5] = IndicesOffset + 5;
			IndicesOffset += 6;
		}

		s_Data.m_IndexBuffer = IndexBuffer::Create(CubeIndices, 12);
		s_Data.m_VertexArray->SetIndexBuffer(s_Data.m_IndexBuffer);
		delete[] CubeIndices;

		//Shader
		s_Data.m_Shader = Shader::Create(AssetManager::GetInstance().GetFullPath("Shaders/Cube.glsl"));
		s_Data.m_Shader->Bind();
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.m_Shader->Bind();
		s_Data.m_VertexArray->Bind();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		s_Data.m_Shader->SetMat4("u_ViewProjection", viewProj);
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		s_Data.m_Shader->Bind();
		s_Data.m_VertexArray->Bind();

		glm::mat4 viewProj = camera.GetViewProjection();

		s_Data.m_Shader->SetMat4("u_ViewProjection", viewProj);
	}

	void Renderer3D::EndScene()
	{
		RenderCommand::DrawIndexed(s_Data.m_VertexArray, 12);
	}

	void Renderer3D::DrawCube(const glm::mat4& transform)
	{
		float CubeVertices[] = {
			//Pos				 Coord		 Normal
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		s_Data.m_VertexBuffer->SetData(CubeVertices, (uint32_t)sizeof(CubeVertices));
		s_Data.m_Shader->SetMat4("u_Model", transform);
	}
}