#pragma once
#include "Runtime/ECS/Component/ComponentBase.h"
#include "Runtime/ECS/Component/Basic/IDComponent.h"
#include "Runtime/ECS/Component/Basic/TagComponent.h"
#include "Runtime/ECS/Component/Camera/CameraComponent.h"
#include "Runtime/ECS/Component/Physics/2D/BoxCollider2DComponent.h"
#include "Runtime/ECS/Component/Physics/2D/CircleCollider2DComponent.h"
#include "Runtime/ECS/Component/Physics/2D/Rigidbody2DComponent.h"
#include "Runtime/ECS/Component/Physics/3D/BoxCollider3DComponent.h"
#include "Runtime/ECS/Component/Physics/3D/Rigidbody3DComponent.h"
#include "Runtime/ECS/Component/Physics/3D/ConvexHullComponent.h"
#include "Runtime/ECS/Component/Physics/3D/SphereCollider3DComponent.h"
#include "Runtime/ECS/Component/Script/NativeScriptComponent.h"
#include "Runtime/ECS/Component/Script/PythonScriptComponent.h"
#include "Runtime/ECS/Component/Shape/CircleRendererComponent.h"
#include "Runtime/ECS/Component/Shape/SpriteRendererComponent.h"
#include "Runtime/ECS/Component/Transform/TransformComponent.h" 
#include "Runtime/ECS/Component/Mesh/MeshComponent.h"
#include "Runtime/ECS/Component/Renderer/PointLightComponent.h"
#include "Runtime/ECS/Component/Renderer/DirectionalLightComponent.h"

#include <concepts>
#include <type_traits>

namespace Flame
{
	// Every Component Class should be registered in this file

	template<class T>
	concept Component = std::is_base_of_v<ComponentBase, T>; //约束必须是ComponentBase的子类

	template<Component... C>
	struct ComponentGroup
	{

	};

	using AllComponents = ComponentGroup<TransformComponent, CircleRendererComponent, SpriteRendererComponent,
		CameraComponent, NativeScriptComponent, PythonScriptComponent, Rigidbody2DComponent, BoxCollider2DComponent,
		CircleCollider2DComponent, Rigidbody3DComponent, BoxCollider3DComponent, SphereCollider3DComponent, ConvexHullComponent,
		MeshComponent, PointLightComponent, DirectionalLightComponent>;
}