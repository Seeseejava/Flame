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
	

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		FLAME_PROFILE_FUNCTION();

		// EventDispatcher������˴���Event�ĺ���, ��Event���͸�ģ��Tƥ��ʱ, ����Ӧ�¼�
		EventDispatcher dispatcher(e);

		// 1. Application����Event, ��e����ΪWindowCloseEventʱ, ����OnWindowClose����
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		// 2. Layer�������¼�, ���������Ϊ����ImGuiLayer�����յ�Event
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.m_Handled)
			{
				break;
			}
		}//backwards(���ﲻ̫��,����ʵ��ʹ���¼�ʱ�ٿ���)
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

	void Application::Init(const ApplicationProps& props)
	{
		Log::Init();

		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(props.Name)));

		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));//�������ڷǾ�̬����
		m_Window->SetVSync(false);//��Ҫ����false������nvida�����������ø�Ϊ�̶�ˢ�£�������G-Vsync compatible



		m_ImGuiLayer = new ImGuiLayer;
		PushOverlay(m_ImGuiLayer);
		Renderer::Init();
	}

	void Application::Run()
	{
		FLAME_PROFILE_FUNCTION();

		while (m_Running)
		{
			FLAME_PROFILE_SCOPE("RunLoop");

			float time = float(glfwGetTime());// Platform::GetTime
			// ע��, ����time - m_LastFrameTIme, �������Ӧ���ǵ�ǰ֡��������ʱ��, ��������һ֡������ʱ��
			
			Timestep timestep = time - m_LastFrameTime; //�����ʼ�����,��m_LastFrameTime��ʼ��Ϊ0�Ϳ�����
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

	void Application::Clean()
	{
		//Renderer::Shutdown();
	}

}