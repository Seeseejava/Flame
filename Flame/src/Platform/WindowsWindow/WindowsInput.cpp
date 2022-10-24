#include "flamepch.h"
#include "Flame/Core/Input.h"

#include "Flame/Core/Application.h"
#include <GLFW/glfw3.h>

//对于Input派生类来说，只有一个子类会被编译到最终的exe里，这是个Compile Time决定的事情，不需要借助虚函数来实现，因为一个Application只可能同时在一个Platform上运行。
//
//所以更好的办法，是在Input类里创建一堆static函数，然后在各个平台对应的Input派生类类实现这些static函数

namespace Flame {

	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();//C++17特性
		
		return (float)x;
	}

	float Input::GetMouseY()
	{
		auto v = GetMousePosition();

		return std::get<0>(v);//C++14
	}

	
}