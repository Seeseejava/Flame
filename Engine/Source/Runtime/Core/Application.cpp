#include "flamepch.h"
#include "Application.h"


#include "Runtime/Core/Log.h"

#include "Runtime/Renderer/Renderer.h"
#include "Runtime/Renderer/RenderCommand.h"

#include "Runtime/Core/Input.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

#include <glfw/glfw3.h>



namespace Flame {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)


	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		// EventDispatcher里面存了处理Event的函数, 在Event类型跟模板T匹配时, 才响应事件
		EventDispatcher dispatcher(e);

		// 1. Application处理Event, 当e类型为WindowCloseEvent时, 调用OnWindowClose函数
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		// 2. Layer来处理事件, 逆序遍历是为了让ImGuiLayer最先收到Event
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
			{
				break;
			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::Init(const ApplicationProps& props)
	{
		Log::Init();

		m_Window = Scope<Window>(Window::Create(WindowProps(props.Name)));

		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//用于类内非静态函数
		m_Window->SetVSync(false);//若要启动false，需在nvida将监视器设置改为固定刷新，而不是G-Vsync compatible



		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);
		Renderer::Init();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = float(glfwGetTime());// Platform::GetTime
			// 注意, 这里time - m_LastFrameTIme, 正好算的应该是当前帧所经历的时间, 而不是上一帧经历的时间
			
			Timestep timestep = time - m_LastFrameTime; // 这里最开始会溢出,将m_LastFrameTime初始化为0就可以了
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			{
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

	void Application::Clean()
	{
		//Renderer::Shutdown();
	}

}