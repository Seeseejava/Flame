#pragma once

#include "Flame/Renderer/OrthographicCamera.h"
#include "Flame/Core/Timestep.h"

#include "Flame/Events/ApplicationEvent.h"
#include "Flame/Events/MouseEvent.h"

namespace Flame {

	// ��ʵ�Ǹ�Camera��Wrapper
	class OrthographicCameraController
	{
	public:
		// �����и��Ƚ��ر�Ĺ�ʽ, ���Ǹ���aspectRatio��zoomLevel���ɶ�Ӧ������ͶӰ����Ĵ�С
		// glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f)����/��*�� = ��
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = m_ZoomLevel, m_CameraRotationSpeed = 180.0f;
	};

}