#pragma once

#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/VertexBuffer.h"
#include "Runtime/Renderer/IndexBuffer.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Material.h"

#define MAX_BONE_INFLUENCE 4

namespace Flame
{
	class Model;
	struct StaticVertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		int EntityID;
	};

	struct SkinnedVertex
	{
		glm::vec3 Pos;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		float m_Weights[MAX_BONE_INFLUENCE];

		int EntityID;
	};

	class Mesh
	{
	public:
		Mesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices);
		Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices);
		Mesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures);
		Mesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures);

		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, const Ref<Shader>& shader, int entityID, Model* model);

		void Draw();
	private:
		void SetupMesh(int entityID);
	private:
		std::vector<StaticVertex> m_StaticVertices;
		std::vector<SkinnedVertex> m_SkinnedVertices;

		std::vector<uint32_t> m_Indices;
		std::vector<MaterialTexture> m_Textures;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		int m_EntityID = -1;
	};
}