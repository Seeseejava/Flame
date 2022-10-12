#pragma once
#pragma once

#include "Flame.h"

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

	struct ProfileResult
	{
		const char* Name;
		float Time;
	};

	std::vector<ProfileResult> m_ProfileResults;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};