#include "flamepch.h"
#include "Application.h"


#include "Flame/Core/Log.h"


#include <GLFW/glfw3.h>
#include "Flame/Renderer/Renderer.h"
#include "Flame/Renderer/RenderCommand.h"

#include "Flame/Core/Input.h"

#include <glfw/glfw3.h>



namespace Flame {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	
	

	Application::Application()
	{
		FLAME_PROFILE_FUNCTION();

		FLAME_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());

		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//用于类内非静态函数
		m_Window->SetVSync(false);//若要启动false，需在nvida将监视器设置改为固定刷新，而不是G-Vsync compatible

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		FLAME_PROFILE_FUNCTION();

		//Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		FLAME_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		FLAME_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	

	void Application::OnEvent(Event& e)
	{
		FLAME_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
			{
				break;
			}
		}//backwards(这里不太懂,到了实际使用事件时再看看)
	}

	void Application::Run()
	{
		FLAME_PROFILE_FUNCTION();

		while (m_Running)
		{
			FLAME_PROFILE_SCOPE("RunLoop");

			float time = float(glfwGetTime());// Platform::GetTime
			// 注意, 这里time - m_LastFrameTIme, 正好算的应该是当前帧所经历的时间, 而不是上一帧经历的时间
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				FLAME_PROFILE_SCOPE("LayerStack OnUpdate");

				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			{
				FLAME_PROFILE_SCOPE("LayerStack OnImGuiRender");

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		FLAME_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}