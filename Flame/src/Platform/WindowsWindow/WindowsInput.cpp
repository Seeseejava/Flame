#include "flamepch.h"
#include "Flame/Core/Input.h"

#include "Flame/Core/Application.h"
#include <GLFW/glfw3.h>

//����Input��������˵��ֻ��һ������ᱻ���뵽���յ�exe����Ǹ�Compile Time���������飬����Ҫ�����麯����ʵ�֣���Ϊһ��Applicationֻ����ͬʱ��һ��Platform�����С�
//
//���Ը��õİ취������Input���ﴴ��һ��static������Ȼ���ڸ���ƽ̨��Ӧ��Input��������ʵ����Щstatic����

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
		auto [x, y] = GetMousePosition();//C++17����
		
		return (float)x;
	}

	float Input::GetMouseY()
	{
		auto v = GetMousePosition();

		return std::get<0>(v);//C++14
	}

	
}