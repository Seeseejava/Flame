#pragma once

#include "Runtime/Core/Core.h"
#include "Runtime/Events/Event.h"
#include "Runtime/Core/Window.h"
#include "Runtime/Core/LayerStack.h"

#include "Runtime/Events/ApplicationEvent.h"

#include "Runtime/ImGui/ImGuiLayer.h"


#include "Runtime/Renderer/OrthographicCamera.h"

#include "Runtime/Core/Timestep.h"

#include "Runtime/Core/PublicSingleton.h"

int main(int argc, char** argv); //非常重要

namespace Flame {

	struct ApplicationProps
	{
		std::string Name;
		uint32_t WindowWidth, WindowHeight;
	};

	class FLAME_API Application : public PublicSingleton<Application>
	{
	public:
		Application() = default;
		virtual ~Application() {}

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void Init(const ApplicationProps& props = { "Flame Engine", 1280, 720 });
		void Run();
		void Clean();
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		friend int ::main(int argc, char** argv);
		// Implemented by CLIENT
		friend void MyAppInitialize(Application& app);
	};
}