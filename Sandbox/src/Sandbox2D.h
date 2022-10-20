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
	Flame::Ref<Flame::Framebuffer> m_Framebuffer;

	Flame::Ref<Flame::Texture2D> m_CheckerboardTexture, m_SpriteSheet;
	Flame::Ref<Flame::SubTexture2D> m_TextureStair, m_TextureGrass, m_TextureTree;


	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleSystem m_ParticleSystem;
	ParticleProperties m_Particle;

	uint32_t m_MapHeight, m_MapWidth;
	std::unordered_map<char, Flame::Ref<Flame::SubTexture2D>> s_TextureMap;
};