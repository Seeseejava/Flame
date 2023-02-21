#pragma once

#include "Runtime/Core/Core.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Mesh.h"
#include "Runtime/Renderer/Texture.h"
#include "Runtime/Renderer/Material.h"
#include "Runtime/Library/TextureLibrary.h"

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

		template <typename Vertex>
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	public:
		bool bUseAlbedoMap = false;
		glm::vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f }; // 0 ~ 1
		Ref<Texture2D> albedoRGBA = Texture2D::Create(1, 1);
		Ref<Texture2D> m_AlbedoMap = Library<Texture2D>::GetInstance().GetDefaultTexture();

		bool bUseNormalMap = false;
		Ref<Texture2D> m_NormalMap = Library<Texture2D>::GetInstance().Get("DefaultNormal");

		bool bUseMetallicMap = false;
		float metallic = 0.1f;
		Ref<Texture2D> metallicRGBA = Texture2D::Create(1, 1);
		Ref<Texture2D> m_MetallicMap = Library<Texture2D>::GetInstance().Get("DefaultMetallicRoughness");

		bool bUseRoughnessMap = false;
		float roughness = 0.1f;
		Ref<Texture2D> roughnessRGBA = Texture2D::Create(1, 1);
		Ref<Texture2D> m_RoughnessMap = Library<Texture2D>::GetInstance().Get("DefaultMetallicRoughness");

		bool bUseAoMap = false;
		Ref<Texture2D> m_AoMap = Library<Texture2D>::GetInstance().GetWhiteTexture();
	private:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory;
		Ref<Material> m_Material = CreateRef<Material>();

		// Animation
		bool bAnimated = false;

	};
}