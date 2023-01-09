#pragma once

#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/VertexBuffer.h"
#include "Runtime/Renderer/IndexBuffer.h"
#include "Runtime/Renderer/Shader.h"

namespace Flame
{
	struct Vertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec2 TexCoord;

		//Editor
		int EntityID;
	};

	class StaticMesh
	{
	public:
		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices);

		void Draw(const glm::mat4& transform, const Ref<Shader>& shader, int entityID);
	private:
		void SetupMesh(int entityID);
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		int m_EntityID = -1;
	};
}