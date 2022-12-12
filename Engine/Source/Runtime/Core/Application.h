#pragma once

#include "Runtime/Core/Core.h"
#include "Runtime/Events/Event.h"
#include "Runtime/Core/Window.h"
#include "Runtime/Core/LayerStack.h"

#include "Runtime/Events/ApplicationEvent.h"

#include "Runtime/ImGui/ImGuiLayer.h"


#include "Runtime/Renderer/OrthographicCamera.h"

#include "Runtime/Core/Timestep.h"


namespace Flame {

	struct ApplicationProps
	{
		std::string Name;
		uint32_t WindowWidth, WindowHeight;
	};

	class FLAME_API Application
	{
	public:
		Application(const ApplicationProps& props = { "Flame Engine", 1280, 720 });
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
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

	// Implemented by CLIENT
	Application* CreateApplication();

}