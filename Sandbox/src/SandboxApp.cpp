#include <Flame.h>
#include <flamepch.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include "imgui/imgui.h"

class ExampleLayer : public Flame::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Flame::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};
		std::shared_ptr<Flame::VertexBuffer> vertexbuffer;
		vertexbuffer.reset(Flame::VertexBuffer::Create(vertices, sizeof(vertices)));

		Flame::BufferLayout layout = {
					{Flame::ShaderDataType::Float3, "a_Position"},
					{Flame::ShaderDataType::Float4, "a_Color"},
					//{ShaderDataType::Float3, "a_Normal"},
		};

		vertexbuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexbuffer);


		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<Flame::IndexBuffer> indexbuffer;
		indexbuffer.reset(Flame::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexbuffer);

		m_SquareVA.reset(Flame::VertexArray::Create());

		float squreVertices[3 * 4] = {
			-0.75f, -0.75f,  0.0f,
			 0.75f, -0.75f,  0.0f,
			 0.75f,  0.75f,  0.0f,
			-0.75f,  0.75f,  0.0f,
		};

		std::shared_ptr<Flame::VertexBuffer> squareVB;
		squareVB.reset(Flame::VertexBuffer::Create(squreVertices, sizeof(squreVertices)));


		Flame::BufferLayout Squarelayout = {
					{Flame::ShaderDataType::Float3, "a_Position"},
					//{ShaderDataType::Float4, "a_Color"},
					//{ShaderDataType::Float3, "a_Normal"},
		};

		squareVB->SetLayout(Squarelayout);

		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int Squareindices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Flame::IndexBuffer> squareIB;
		squareIB.reset(Flame::IndexBuffer::Create(Squareindices, sizeof(Squareindices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position  = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader.reset(new Flame::Shader(vertexSrc, fragmentSrc));

		std::string vertexSrc2 = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position  = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1);
			}
		)";
		m_Shader2.reset(new Flame::Shader(vertexSrc2, fragmentSrc2));
	}

	void OnUpdate() override
	{
		if (Flame::Input::IsKeyPressed(FLAME_KEY_TAB))
			FLAME_TRACE("Tab key is pressed (poll)!");
		if (Flame::Input::IsKeyPressed(FLAME_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed;
		else if (Flame::Input::IsKeyPressed(FLAME_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed;

		if (Flame::Input::IsKeyPressed(FLAME_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed;
		else if (Flame::Input::IsKeyPressed(FLAME_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed;

		if (Flame::Input::IsKeyPressed(FLAME_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed;
		if (Flame::Input::IsKeyPressed(FLAME_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed;

		Flame::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Flame::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Flame::Renderer::BeginScene(m_Camera);

		Flame::Renderer::Submit(m_Shader2, m_SquareVA);
		Flame::Renderer::Submit(m_Shader, m_VertexArray);

		Flame::Renderer::EndScene();
	}

	private:
		std::shared_ptr<Flame::Shader> m_Shader;//这里要把unique变成shared，为了更安全？
		std::shared_ptr<Flame::VertexArray> m_VertexArray;

		std::shared_ptr<Flame::Shader> m_Shader2;
		std::shared_ptr<Flame::VertexArray> m_SquareVA;

		Flame::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition;
		float m_CameraMoveSpeed = 0.005f;

		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 0.1f;

	virtual void OnImGuiRender() override
	{

	}
	void OnEvent(Flame::Event& event) override
	{

	}
};

class Sandbox : public Flame::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Flame::Application* Flame::CreateApplication()
{
	return new Sandbox();
}