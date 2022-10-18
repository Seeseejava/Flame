#pragma once
#pragma once

#include "Flame.h"

#include "ParticleSystem.h"

class Sandbox2D : public Flame::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Flame::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Flame::Event& e) override;
private:
	Flame::OrthographicCameraController m_CameraController;

	// Temp
	Flame::Ref<Flame::VertexArray> m_SquareVA;
	Flame::Ref<Flame::Shader> m_FlatColorShader;
	Flame::Ref<Flame::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleSystem m_ParticleSystem;
	ParticleProperties m_Particle;
};