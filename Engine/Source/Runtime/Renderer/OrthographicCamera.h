#pragma once

#include "glm/glm.hpp"

namespace Flame {

	class OrthographicCamera
	{
	public:
		// 默认near为-1, far为1
		OrthographicCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		float GetRotation()const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;// 作为计算时的Cache

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;// 正交投影下的相机只会有绕Z轴的旋转
	};
}