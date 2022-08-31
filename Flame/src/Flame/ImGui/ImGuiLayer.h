#pragma once

#include "Flame/Layer.h"

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
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	
	private:
		float m_Time = 0.0f;
	};
}