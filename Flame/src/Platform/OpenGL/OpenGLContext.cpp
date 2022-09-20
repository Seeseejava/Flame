#include "flamepch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Flame {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		FLAME_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		// glad: load all OpenGL function pointers
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FLAME_CORE_ASSERT(status, "Failed to initialize Glad!");

		FLAME_CORE_INFO("OpenGL Info:");
		FLAME_CORE_INFO("Vendeor: {0}", glGetString(GL_VENDOR));
		FLAME_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		FLAME_CORE_INFO("Version: {0}", glGetString(GL_VERSION));


	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}