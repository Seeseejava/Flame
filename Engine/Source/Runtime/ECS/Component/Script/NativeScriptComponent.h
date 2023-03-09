//目前作用未知
#pragma once
#include "Runtime/ECS/Component/ComponentBase.h"

namespace Flame
{
	// Forward declaration
	class ScriptableEntity;
	class NativeScriptComponent : public ComponentBase
	{
	public:
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void(*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
}