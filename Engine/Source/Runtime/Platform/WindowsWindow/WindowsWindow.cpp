#include "flamepch.h"
#include "WindowsWindow.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Events/ApplicationEvent.h"
#include "Runtime/Events/MouseEvent.h"
#include "Runtime/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include <stb_image/stb_image.h>

namespace Flame {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		FLAME_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		FLAME_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		FLAME_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		FLAME_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		FLAME_CORE_INFO("Creating window {0} ({1},{2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			FLAME_PROFILE_SCOPE("glfwInit");

			int success = glfwInit();
			FLAME_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		{
			FLAME_PROFILE_SCOPE("glfwCreateWindow");

			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		}

		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		/*每个窗口具有可与设置用户指针glfwSetWindowUserPointer并用查询glfwGetWindowUserPointer。
		这可以用于您需要的任何目的, 并且在窗口的整个生命周期内不会被GLFW修改。
		指针的初始值是NULL。*/

		SetVSync(true);

		GLFWimage images[1];
		images[0].pixels = stbi_load(AssetManager::GetFullPath("Resources/Icon/Flame2.jpg").string().c_str(), &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(m_Window, 1, images);
		stbi_image_free(images[0].pixels);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;//放在前面以防未更新

				WindowResizeEvent event(width, height);
				//FLAME_CORE_WARN("{0}, {1}", width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keycode);
				data.EventCallback(event);

			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xoffset, (float)yoffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xpos, (float)ypos);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		FLAME_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		FLAME_PROFILE_FUNCTION();

		glfwPollEvents(); 
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		FLAME_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		/*如果间隔为零, 则在调用glfwSwapBuffers时立即进行交换, 而不等待刷新。
			否则, 至少间隔回溯将在每个缓冲区交换之间传递。
			当不希望测量等待垂直回扫所花费的时间时, 使用零交换间隔对于基准测试目的是有用的。
			但是, 交换间隔为1可以避免撕裂。*/

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}