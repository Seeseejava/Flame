#pragma once

#include "Flame.h"

class ExampleLayer : public Flame::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Flame::Timestep ts) override;

	virtual void OnImGuiRender() override;
	void OnEvent(Flame::Event& e) override;
private:
	Flame::ShaderLibrary m_ShaderLibrary;
	Flame::Ref<Flame::Shader> m_Shader;//这里要把unique变成shared，为了更安全？
	Flame::Ref<Flame::VertexArray> m_VertexArray;

	Flame::Ref<Flame::Shader> m_FlatColorShader;
	Flame::Ref<Flame::VertexArray> m_SquareVA;

	Flame::Ref<Flame::Texture2D> m_Texture, m_ChernoLogoTexture;

	Flame::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquarePosition;
	float m_SquareMoveSpeed = 5.0f;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.7 };

};