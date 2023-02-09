#pragma once


#include "Runtime/Camera/EditorCamera.h"
#include "Runtime/ECS/Component/Mesh/StaticMeshComponent.h"
#include "Runtime/Renderer/Texture.h"

namespace Flame
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		static void DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, int EntityID);

		static Ref<CubeMapTexture> GetSkyBox();
		static Ref<CubeMapTexture> GetDefaultSkyBox();
	};
}