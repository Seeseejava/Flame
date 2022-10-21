#pragma once

#include "Flame/Renderer/OrthographicCamera.h"
#include "Flame/Core/Timestep.h"

#include "Flame/Events/ApplicationEvent.h"
#include "Flame/Events/MouseEvent.h"

namespace Flame {

	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	// ��ʵ�Ǹ�Camera��Wrapper
	class OrthographicCameraController
	{
	public:
		// �����и��Ƚ��ر�Ĺ�ʽ, ���Ǹ���aspectRatio��zoomLevel���ɶ�Ӧ������ͶӰ����Ĵ�С
		// glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f)����/��*�� = ��
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; CalculateView(); }

		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }
	private:
		void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = m_ZoomLevel, m_CameraRotationSpeed = 180.0f;
	};

}