#pragma once
#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/ECS/Component/Basic/IDComponent.h"
#include "Runtime/ECS/Component/Basic/TagComponent.h"
#include "Runtime/ECS/Component/Camera/CameraComponent.h"
#include "Runtime/ECS/Component/Physics/BoxCollider2DComponent.h"
#include "Runtime/ECS/Component/Physics/CircleCollider2DComponent.h"
#include "Runtime/ECS/Component/Physics/Rigidbody2DComponent.h"
#include "Runtime/ECS/Component/Script/NativeScriptComponent.h"
#include "Runtime/ECS/Component/Shape/CircleRendererComponent.h"
#include "Runtime/ECS/Component/Shape/SpriteRendererComponent.h"
#include "Runtime/ECS/Component/Transform/TransformComponent.h" 

#include <concepts>
#include <type_traits>

namespace Flame
{
	//template<class T>
	//concept Component = std::is_base_o

	template<typename... Component>
	struct ComponentGroup
	{

	};
}