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
		FLAME_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		// glad: load all OpenGL function pointers
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		FLAME_CORE_ASSERT(status, "Failed to initialize Glad!");

		FLAME_CORE_INFO("OpenGL Info:");
		FLAME_CORE_INFO("Vendeor: {0}", glGetString(GL_VENDOR));
		FLAME_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		FLAME_CORE_INFO("Version: {0}", glGetString(GL_VERSION));

		// ¼ì²éOpenGL°æ±¾
		#ifdef FLAME_ENABLE_ASSERTS
				int versionMajor;
				int versionMinor;
				glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
				glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

				FLAME_CORE_ASSERT((versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5)), "Hazel requires at least OpenGL version 4.5!");
		#endif

	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}