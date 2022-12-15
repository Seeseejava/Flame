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
	// Every Component Class should be registered in this file

	template<class T>
	concept Component = std::is_base_of_v<ComponentBase, T>; //Լ��������ComponentBase������

	template<Component... C>
	struct ComponentGroup
	{

	};

	using AllComponents = ComponentGroup<TransformComponent, CircleRendererComponent, SpriteRendererComponent,
		CameraComponent, NativeScriptComponent,
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent>;
}