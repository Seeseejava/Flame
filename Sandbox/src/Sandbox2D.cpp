#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.f / 720.f, true)
{
}

void Sandbox2D::OnAttach()
{
	FLAME_PROFILE_FUNCTION();

	m_CheckerboardTexture = Flame::Texture2D::Create("assets/texture/Checkerboard.png");
	m_SpriteSheet = Flame::Texture2D::Create("assets/RPGGame/texture/RPGpack_sheet_2X.png");

	m_TextureGrass = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 3 }, { 128, 128 });
	m_TextureStair = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7 , 6 }, { 128, 128 });
	m_TextureTree = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 1 }, { 128, 128 }, { 1, 2 });

	// Init here
	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 1.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(5.0f);
}

void Sandbox2D::OnDetach()
{
	FLAME_PROFILE_FUNCTION();

}

void Sandbox2D::OnUpdate(Flame::Timestep ts)
{
	FLAME_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Flame::Renderer2D::ResetStats();
	{	
		FLAME_PROFILE_SCOPE("Renderer Prep");
		Flame::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Flame::RenderCommand::Clear();
	}

	{	

		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		FLAME_PROFILE_SCOPE("Renderer Draw");

#if 0
		Flame::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Flame::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Flame::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Flame::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Flame::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		Flame::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
		Flame::Renderer2D::EndScene();

		Flame::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Flame::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Flame::Renderer2D::EndScene();
#endif

		Flame::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Flame::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureGrass);
		Flame::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStair);
		Flame::Renderer2D::DrawQuad({ 2.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);
		Flame::Renderer2D::EndScene();


	}

	if (Flame::Input::IsMouseButtonPressed(FLAME_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Flame::Input::GetMousePosition();
		auto width = Flame::Application::Get().GetWindow().GetWidth();
		auto height = Flame::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
	
}

void Sandbox2D::OnImGuiRender()
{
	FLAME_PROFILE_FUNCTION();
	ImGui::Begin("Settings");

	auto stats = Flame::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Flame::Event& e)
{
	m_CameraController.OnEvent(e);
}

