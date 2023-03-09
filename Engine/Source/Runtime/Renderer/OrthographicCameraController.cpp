#include "flamepch.h"
#include "OrthographicCameraController.h"

#include "Runtime/Core/Input.h"
#include "Runtime/Core/KeyCodes.h"

namespace Flame {
	
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }), m_Camera(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top), m_Rotation(rotation)
	{
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		FLAME_PROFILE_FUNCTION();

		if (Input::IsKeyPressed(FLAME_KEY_A))
			m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(FLAME_KEY_D))
			m_CameraPosition.x += m_CameraTranslationSpeed * ts;

		if (Input::IsKeyPressed(FLAME_KEY_W))
			m_CameraPosition.y += m_CameraTranslationSpeed * ts;
		else if (Input::IsKeyPressed(FLAME_KEY_S))
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts;

		if (m_Rotation)
		{
			if (Input::IsKeyPressed(FLAME_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (Input::IsKeyPressed(FLAME_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		FLAME_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(FLAME_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(FLAME_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::CalculateView()
	{
		m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };
		m_Camera.SetProjection(m_Bounds.Left, m_Bounds.Right, m_Bounds.Bottom, m_Bounds.Top);
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		// 里面会调用glm::ortho(float left, float right, float bottom, float top)函数
	    // 根据这段代码可知, camera看到的区域高度不会随着窗口大小而改变
		// 而是会随着鼠标滚动改变zoom值而变化
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	} 

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		FLAME_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		CalculateView();
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		FLAME_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}