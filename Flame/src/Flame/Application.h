#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "LayerStack.h"

#include "Events/ApplicationEvent.h"

#include "Flame/ImGui/ImGuiLayer.h"


#include "Flame/Renderer/OrthographicCamera.h"

#include "Flame/Core/Timestep.h"

namespace Flame {

	class FLAME_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window;}
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime;

		
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}