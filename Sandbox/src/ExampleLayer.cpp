#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

	ExampleLayer::ExampleLayer()
		:Layer("Example"), m_CameraController(1280.f / 720.f, true)
	{
		m_VertexArray = Flame::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};
		Flame::Ref<Flame::VertexBuffer> vertexbuffer;
		vertexbuffer = Flame::VertexBuffer::Create(vertices, sizeof(vertices));

		Flame::BufferLayout layout = {
					{Flame::ShaderDataType::Float3, "a_Position"},
					{Flame::ShaderDataType::Float4, "a_Color"},
					//{ShaderDataType::Float3, "a_Normal"},
		};

		vertexbuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(vertexbuffer);


		unsigned int indices[3] = { 0, 1, 2 };
		Flame::Ref<Flame::IndexBuffer> indexbuffer;
		indexbuffer = Flame::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(indexbuffer);

		m_SquareVA = Flame::VertexArray::Create();

		float squreVertices[5 * 4] = {
			-0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f, 0.0f, 1.0f
		};

		Flame::Ref<Flame::VertexBuffer> squareVB;
		squareVB = Flame::VertexBuffer::Create(squreVertices, sizeof(squreVertices));


		Flame::BufferLayout Squarelayout = {
					{Flame::ShaderDataType::Float3, "a_Position"},
					{Flame::ShaderDataType::Float2, "a_TexCoord"},
					//{ShaderDataType::Float3, "a_Normal"},
		};

		squareVB->SetLayout(Squarelayout);

		m_SquareVA->AddVertexBuffer(squareVB);

		unsigned int Squareindices[6] = { 0, 1, 2, 2, 3, 0 };
		Flame::Ref<Flame::IndexBuffer> squareIB;
		squareIB = Flame::IndexBuffer::Create(Squareindices, sizeof(Squareindices) / sizeof(uint32_t));
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
		m_Shader = Flame::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

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
		m_FlatColorShader = Flame::Shader::Create("FlatColor", vertexSrc2, fragmentSrc2);

		auto textureShader = m_ShaderLibrary.Load("assets/shader/Texture.glsl");

		m_Texture = Flame::Texture2D::Create("assets/texture/Checkerboard.png");//此时返回的是智能指针，不再需要使用reset
		m_ChernoLogoTexture = Flame::Texture2D::Create("assets/texture/ChernoLogo.png");

		textureShader->Bind();
		textureShader->SetInt("u_Texture", 0);
	}

	void ExampleLayer::OnAttach()
	{
	}

	void ExampleLayer::OnDetach()
	{
	}

	void ExampleLayer::OnUpdate(Flame::Timestep ts) 
	{
		//FLAME_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());
		// Update
		m_CameraController.OnUpdate(ts);
		// Render
		Flame::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Flame::RenderCommand::Clear();

		Flame::Renderer::BeginScene(m_CameraController.GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));//使用static不用每次都算


		m_FlatColorShader->Bind();
		m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);
		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < 20; i++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Flame::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}

		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Flame::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_ChernoLogoTexture->Bind();
		Flame::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		// Triangle
		//Flame::Renderer::Submit(m_Shader, m_VertexArray);

		Flame::Renderer::EndScene();
	}

	void ExampleLayer::OnImGuiRender()
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}
	void ExampleLayer::OnEvent(Flame::Event& e) 
	{
		m_CameraController.OnEvent(e);
	}