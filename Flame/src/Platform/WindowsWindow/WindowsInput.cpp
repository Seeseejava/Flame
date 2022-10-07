#include "flamepch.h"
#include "WindowsInput.h"

#include "Flame/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Flame {

	Input* Input::s_Instance = new WindowsInput();//���Ǻ����ΪʲôҪ���

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();//C++17����
		
		return (float)x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto v = GetMousePositionImpl();

		return std::get<0>(v);//C++14
	}

	
}