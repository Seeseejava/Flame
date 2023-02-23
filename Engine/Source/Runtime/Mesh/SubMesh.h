#pragma once

#include "Runtime/Renderer/VertexArray.h"
#include "Runtime/Renderer/VertexBuffer.h"
#include "Runtime/Renderer/IndexBuffer.h"
#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Material.h"

#define MAX_BONE_INFLUENCE 4

namespace Flame
{
	class Mesh;
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

		int EntityID;

		int m_BoneIDs[MAX_BONE_INFLUENCE];
		float m_Weights[MAX_BONE_INFLUENCE];
	};

	class SubMesh
	{
	public:
		SubMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices);
		SubMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices);
		SubMesh(const std::vector<StaticVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures, uint32_t materialIndex = 0);
		SubMesh(const std::vector<SkinnedVertex>& vertices, const std::vector<uint32_t> indices, const std::vector<MaterialTexture>& textures, uint32_t materialIndex = 0);

		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, const Ref<Shader>& shader, int entityID, Mesh* model);

		void Draw();
	private:
		void SetupMesh(int entityID);

	public:
		uint32_t m_MaterialIndex;
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