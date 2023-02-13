#pragma once

#include "Runtime/Core/Core.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/StaticMesh.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Flame
{
	class Model
	{
	public:
		Model() = default;
		Model(const std::string& path)
			: m_Material(CreateRef<Material>(Library<Shader>::GetInstance().GetDefaultShader()))
		{
			LoadModel(path);
		}

		Model(const std::string& path, Ref<Shader> shader)
			: m_Material(CreateRef<Material>(shader))
		{
			LoadModel(path);
		}

		void SetShader(Ref<Shader> shader) { m_Material->SetShader(shader); };
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, int entityID);
		void Draw(const glm::mat4& transform, const glm::vec3& cameraPos, Ref<Shader> shader, int entityID);
		void Draw();

	private:

		std::optional<std::vector<MaterialTexture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		StaticMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<StaticMesh> m_Meshes;
		std::string m_Directory;
		Ref<Material> m_Material = CreateRef<Material>();

	};
}