#pragma once
#pragma once

#include "Flame.h"

#include "ParticleSystem.h"

namespace Flame {
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		OrthographicCameraController m_CameraController;

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		entt::entity m_SquareEntity;

		Ref<Texture2D> m_CheckerboardTexture, m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStair, m_TextureGrass, m_TextureTree;


		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		ParticleSystem m_ParticleSystem;
		ParticleProperties m_Particle;

		uint32_t m_MapHeight, m_MapWidth;
		std::unordered_map<char, Ref<SubTexture2D>> s_TextureMap;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_LastViewportSize = { 0 , 0 };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	};
}