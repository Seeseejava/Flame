#pragma once

#include "Runtime/Core/Core.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/StaticMesh.h"

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
		{
			LoadModel(path);
		}

		void Draw(const glm::mat4& transform, Ref<Shader>& shader, int entityID);

		void Draw();

	private:
		std::vector<StaticMesh> m_Meshes;
		std::string m_directory;

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		StaticMesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	};
}