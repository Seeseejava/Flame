#pragma once

#include "Flame/Core/Core.h"
#include "Flame/Events/Event.h"
#include "Flame/Core/Window.h"
#include "Flame/Core/LayerStack.h"

#include "Flame/Events/ApplicationEvent.h"

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

		void Close();

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window;}
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		
	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}