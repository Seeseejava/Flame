#pragma once

#include "Flame/Renderer/Texture.h"

namespace Flame {

	class ToolBarPanel
	{
	public:
		ToolBarPanel();

		void OnScenePlay();
		void OnSceneStop();

		void OnImGuiRender();
	private:

		Ref<Texture2D> m_PlayIcon;
		Ref<Texture2D> m_StopIcon;
	public:
		enum class SceneState
		{
			Edit = 0, Play = 1
		};
		SceneState m_SceneState = SceneState::Edit;
	};

}