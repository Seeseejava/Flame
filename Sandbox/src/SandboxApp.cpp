#include <Flame.h>
#include <flamepch.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp> //glm::value_ptr

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

class ExampleLayer : public Flame::Layer
{
public:
	ExampleLayer()
		:Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
	{
		m_VertexArray.reset(Flame::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};
		Flame::Ref<Flame::VertexBuffer> vertexbuffer;
		vertexbuffer.reset(Flame::VertexBuffer::Create(vertices, sizeof(vertices)));

		Flame::BufferLayout layout = {
					{Flame::ShaderDataType::Float3, "a_Position"},
					{Flame::ShaderDataType::Float4, "a_Color"},
					//{ShaderDataType::Float3, "a_Normal"},
		};

		vertexbuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexbuffer);


		unsigned int indices[3] = { 0, 1, 2 };
		Flame::Ref<Flame::IndexBuffer> indexbuffer;
		indexbuffer.reset(Flame::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexbuffer);

		m_SquareVA.reset(Flame::VertexArray::Create());

		float squreVertices[5 * 4] = {
			-0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f, 0.0f, 1.0f
		};

		Flame::Ref<Flame::VertexBuffer> squareVB;
		squareVB.reset(Flame::VertexBuffer::Create(squreVertices, sizeof(squreVertices)));


		Flame::BufferLayout Squarelayout = {
					{Flame::ShaderDataType::Float3, "a_Position"},
					{Flame::ShaderDataType::Float2, "a_TexCoord"},
					//{ShaderDataType::Float3, "a_Normal"},
		};

		squareVB->SetLayout(Squarelayout);

		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int Squareindices[6] = { 0, 1, 2, 2, 3, 0 };
		Flame::Ref<Flame::IndexBuffer> squareIB;
		squareIB.reset(Flame::IndexBuffer::Create(Squareindices, sizeof(Squareindices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position  = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		//m_Shader.reset(new Flame::Shader(vertexSrc, fragmentSrc)); 这里不能再用new了，因为此时shader已经是一个抽象类
		m_Shader.reset(Flame::Shader::Create(vertexSrc, fragmentSrc));

		std::string vertexSrc2 = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position  = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0f);
			}
		)";
		m_Shader2.reset(Flame::Shader::Create(vertexSrc2, fragmentSrc2));

		std::string textureShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec2 v_TexCoord;

			void main()
			{
				v_TexCoord  = a_TexCoord;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string textureShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 v_TexCoord;

			uniform sampler2D u_Texture;

			void main()
			{
				color = texture(u_Texture, v_TexCoord);
			}
		)";
		m_TextureShader.reset(Flame::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));

		m_Texture = Flame::Texture2D::Create("assets/texture/Checkerboard.png");//此时返回的是智能指针，不再需要使用reset

		std::dynamic_pointer_cast<Flame::OpenGLShader>(m_TextureShader)->Bind();//下行可检测，安全
		std::dynamic_pointer_cast<Flame::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);//因为绑定到了slot 0
	}

	void OnUpdate(Flame::Timestep ts) override
	{
		//FLAME_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());

		if (Flame::Input::IsKeyPressed(FLAME_KEY_TAB))
			FLAME_TRACE("Tab key is pressed (poll)!");
		if (Flame::Input::IsKeyPressed(FLAME_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (Flame::Input::IsKeyPressed(FLAME_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (Flame::Input::IsKeyPressed(FLAME_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Flame::Input::IsKeyPressed(FLAME_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Flame::Input::IsKeyPressed(FLAME_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		if (Flame::Input::IsKeyPressed(FLAME_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;



		/*if (Flame::Input::IsKeyPressed(FLAME_KEY_J))
			m_SquarePosition.x -= m_SquareMoveSpeed * ts;
		else if (Flame::Input::IsKeyPressed(FLAME_KEY_L))
			m_SquarePosition.x += m_SquareMoveSpeed * ts;

		if (Flame::Input::IsKeyPressed(FLAME_KEY_I))
			m_SquarePosition.y += m_SquareMoveSpeed * ts;
		else if (Flame::Input::IsKeyPressed(FLAME_KEY_K))
			m_SquarePosition.y -= m_SquareMoveSpeed * ts;*/


		Flame::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Flame::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Flame::Renderer::BeginScene(m_Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));//使用static不用每次都算


		std::dynamic_pointer_cast<Flame::OpenGLShader>(m_Shader2)->Bind();//下行可检测，安全
		std::dynamic_pointer_cast<Flame::OpenGLShader>(m_Shader2)->UploadUniformFloat3("u_Color", m_SquareColor);
		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < 20; i++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Flame::Renderer::Submit(m_Shader2, m_SquareVA, transform);
			}

		}

		m_Texture->Bind();
		Flame::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Triangle
		//Flame::Renderer::Submit(m_Shader, m_VertexArray);

		Flame::Renderer::EndScene();
	}

	private:
		Flame::Ref<Flame::Shader> m_Shader;//这里要把unique变成shared，为了更安全？
		Flame::Ref<Flame::VertexArray> m_VertexArray;

		Flame::Ref<Flame::Shader> m_Shader2, m_TextureShader;
		Flame::Ref<Flame::VertexArray> m_SquareVA;

		Flame::Ref<Flame::Texture2D> m_Texture;

		Flame::OrthographicCamera m_Camera;
		glm::vec3 m_CameraPosition;
		float m_CameraMoveSpeed = 5.0f;

		float m_CameraRotation = 0.0f;
		float m_CameraRotationSpeed = 180.0f;

		glm::vec3 m_SquarePosition;
		float m_SquareMoveSpeed = 5.0f;
		glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.7 };

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
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