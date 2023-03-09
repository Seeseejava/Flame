#pragma once

#include "Runtime/Core/Core.h"
#include "Runtime/Animation/animdata.h"
#include "Runtime/Animation/animator.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Mesh/SubMesh.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <optional>
#include <map>

namespace Flame
{
	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const Mesh&) = default;
		Mesh(const std::string& path)
		{
			m_Material.push_back(CreateRef<Material>(Library<Shader>::GetInstance().GetDefaultShader()));
			LoadModel(path);
		}

		Mesh(const std::string& path, Ref<Shader> shader)
		{
			m_Material.push_back(CreateRef<Material>(shader));
			LoadModel(path);
		}

		void SetShader(Ref<Shader> shader) { m_Material[0]->SetShader(shader); };
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, int entityID);
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, Ref<Shader> shader, int entityID);
		void Draw();

		auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		int& GetBoneCount() { return m_BoneCounter; }

	private:

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, uint32_t& subMeshIndex);

		template <typename Vertex>
		SubMesh ProcessMesh(aiMesh* mesh, const aiScene* scene, uint32_t& subMeshIndex);
		std::optional<std::vector<MaterialTexture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, uint32_t& subMeshIndex);

	public:
		// Animation
		bool bAnimated = false;
		bool bPlayAnim = false;
		bool bStopAnim = false;

		Animation m_Animation;
		Animator m_Animator;

		float m_AnimPlaySpeed = 1.0f;

		std::vector<Ref<Material>> m_Material;
		std::vector<SubMesh> m_SubMeshes;
	private:
		std::string m_Directory;

		// Animation
		int m_BoneCounter = 0;
		std::map<std::string, BoneInfo> m_BoneInfoMap;

	};
}