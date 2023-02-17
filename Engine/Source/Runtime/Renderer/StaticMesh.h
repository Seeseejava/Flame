#pragma once

#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/VertexBuffer.h"
#include "Runtime/Renderer/IndexBuffer.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Material.h"

namespace Flame
{
	class Model;
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
		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures);

		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos,  const Ref<Shader>& shader, int entityID, Model* model);

		void Draw();
	private:
		void SetupMesh(int entityID);
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<MaterialTexture> m_Textures;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		int m_EntityID = -1;
	};
}