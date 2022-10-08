#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.f / 720.f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_SquareVA = Flame::VertexArray::Create();

	float squreVertices[3 * 4] = {
		-0.5f, -0.5f,  0.0f, 
		 0.5f, -0.5f,  0.0f, 
		 0.5f,  0.5f,  0.0f, 
		-0.5f,  0.5f,  0.0f, 
	};

	Flame::Ref<Flame::VertexBuffer> squareVB;
	squareVB = Flame::VertexBuffer::Create(squreVertices, sizeof(squreVertices));

	Flame::BufferLayout Squarelayout = {
				{Flame::ShaderDataType::Float3, "a_Position"},
	};
	squareVB->SetLayout(Squarelayout);
	m_SquareVA->AddVertexBuffer(squareVB);

	unsigned int Squareindices[6] = { 0, 1, 2, 2, 3, 0 };
	Flame::Ref<Flame::IndexBuffer> squareIB;
	squareIB = Flame::IndexBuffer::Create(Squareindices, sizeof(Squareindices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = Flame::Shader::Create("assets/shader/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Flame::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);
	// Render
	Flame::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Flame::RenderCommand::Clear();

	Flame::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Flame::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Flame::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

	Flame::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Flame::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Flame::Event& e)
{
	m_CameraController.OnEvent(e);
}

