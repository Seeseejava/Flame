#pragma once

#include "Flame/Core/Layer.h"

#include "Flame/Events/MouseEvent.h"
#include "Flame/Events/KeyEvent.h"
#include "Flame/Events/ApplicationEvent.h"

namespace Flame {

	class FLAME_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event & e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();
	
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}