#pragma once

#include "Runtime/ECS/Component/ComponentBase.h"

#include <fmod.hpp>

namespace Flame
{
	class SoundComponent : public ComponentBase
	{
	public:
		SoundComponent() = default;
		SoundComponent(const SoundComponent&) = default;
		SoundComponent(const std::string& path)
			: Path(path)
		{}

		std::string Path;
		bool Play;

		FMOD::Sound* Sound;
		FMOD::Channel* Channel = nullptr;
	};
}