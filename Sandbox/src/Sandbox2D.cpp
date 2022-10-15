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
	{	
		FLAME_PROFILE_SCOPE("Renderer Prep");
		Flame::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Flame::RenderCommand::Clear();
	}

	{	
		FLAME_PROFILE_SCOPE("Renderer Draw");
		Flame::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Flame::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Flame::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		//Flame::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);
		Flame::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		Flame::Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, m_CheckerboardTexture, 20.0f);

		Flame::Renderer2D::EndScene();
	}
	
}

void Sandbox2D::OnImGuiRender()
{
	FLAME_PROFILE_FUNCTION();
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Flame::Event& e)
{
	m_CameraController.OnEvent(e);
}

