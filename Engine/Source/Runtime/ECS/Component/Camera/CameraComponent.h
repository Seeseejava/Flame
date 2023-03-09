#pragma once
#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/Camera/SceneCamera.h"

namespace Flame
{
	class CameraComponent : public ComponentBase
	{
	public:
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to Scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}