#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace Flame {
	static const uint32_t s_MapWidth = 16;
	static const char s_MapTiles[] =
	{
		// ����д����ʵ����һ�����ַ���, D����Dirt����Tile, W����Water Tile, S����·��Tile
		// ע���һ��TileΪD, ��Ȼ������������Ϊ(0,0), ��������Ļ�϶�Ӧ������Ӧ����(0,1)
		"DDWWWWWWWWWWWWWW"
		"DDWWWWWWWWWWWWWW"
		"DDDDDDDDDDDWWWWW"
		"DDDDDSDDDDDWWWWW"
		"DDDDDDDDDDDWWWWW"
		"DDWWWWWWWWWWWWWW"
		"DDWWWWWWWWWDDSDD"
		"DDWWWWWWWWWWWWWW"
		"DDWWWWWWWWWWWWWW"
	};

	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true)
	{
	}

	void EditorLayer::OnAttach()
	{
		FLAME_PROFILE_FUNCTION();

		m_CheckerboardTexture = Texture2D::Create("assets/texture/Checkerboard.png");
		m_SpriteSheet = Texture2D::Create("assets/RPGGame/texture/RPGpack_sheet_2X.png");

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = std::make_shared<Scene>();

		// Entity
		m_SquareEntity = m_ActiveScene->CreateEntity("Green Square");
		m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		m_MapWidth = s_MapWidth;
		m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
		s_TextureMap['D'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6 , 11 }, { 128, 128 });
		s_TextureMap['W'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11 , 11 }, { 128, 128 });
		s_TextureMap['S'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1 , 11 }, { 128, 128 });

		m_TextureStair = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
		m_TextureGrass = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 3 }, { 128, 128 });
		m_TextureTree = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 1 }, { 128, 128 }, { 1, 2 });

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

	void EditorLayer::OnDetach()
	{
		FLAME_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		FLAME_PROFILE_FUNCTION();

		// Resize
		// �ı�Viewport���ڴ�С����ֱ�����������������ţ�����ֱ�������Ұ���䣻ˮƽ���������Ҳ�����ţ���ˮƽ�����������ҰҲ��仯
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts); //���¼����¼�

		// Render
		Renderer2D::ResetStats();
		{
			FLAME_PROFILE_SCOPE("Renderer Prep");
			//���Ĭ��frambuffer
			Flame::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));// Ĭ�ϴ�����ɫ��Ϊmagenta
			Flame::RenderCommand::Clear();
			m_Framebuffer->Bind();
			// clear�ҵ�Framebuffer
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();
		}

		{
			FLAME_PROFILE_SCOPE("Renderer Draw");

			// ��ͼ
			//for (uint32_t y = 0; y < m_MapHeight; y++)
			//{
			//	for (uint32_t x = 0; x < m_MapWidth; x++)
			//	{
			//		char tileType = s_MapTiles[x + y * m_MapWidth];
			//		Ref<SubTexture2D> texture;

			//		if (s_TextureMap.find(tileType) != s_TextureMap.end())
			//			texture = s_TextureMap[tileType];
			//		else
			//			texture = m_TextureGrass;

			//		Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight / 2.0f - y, 0.1f }, { 1.0f,1.0f }, texture);// ������Ҫ��תһ��y��
			//	}
			//}

			/*Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureGrass);
			Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStair);
			Renderer2D::DrawQuad({ 2.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);*/


			m_ActiveScene->OnUpdate(ts);


		}

		if (Input::IsMouseButtonPressed(FLAME_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Input::GetMousePosition();						//���λ��������ڴ������Ͻǣ�����������Ϊ������λΪ����
			auto width = Application::Get().GetWindow().GetWidth();
			auto height = Application::Get().GetWindow().GetHeight();	//���ڴ�С��1280x720

			auto bounds = m_CameraController.GetBounds();						//��zoomlevelʱ��left = -5 * 1280 / 720, bottom = -5;
			auto pos = m_CameraController.GetCamera().GetPosition();			//�������δ�ƶ�ʱΪ0
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;		//ǰһ��������������������½ǵ�λ�ã������������õ�������������λ�ã��任��������������ˣ�
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());
		m_Framebuffer->Unbind(); //Ŀǰ����ϵͳ�����λ��������

	}

	void EditorLayer::OnImGuiRender()
	{
		FLAME_PROFILE_FUNCTION();

		static bool docspaceOpen = true;//?
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &docspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.

				if (ImGui::MenuItem("Exit")) Application::Get().Close();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}


		ImGui::Begin("Settings");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		if (m_SquareEntity)
		{
			ImGui::Separator();
			auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("%s", tag.c_str());

			auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
			ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
			ImGui::Separator();
		}

		ImGui::DragFloat3("Camera Transform",
			glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3])); // mat4��4��4�е�Ԫ���ǰ����еķ�ʽ���е�:��Ҫ����Ϊopengl���Դ����ǰ��д洢��

		if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}
		{
			auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);
		}
		ImGui::End();

		//���ô��ڵ�paddingΪ0��ͼƬ�ؼ���������
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//���ڳߴ�
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		// This solution will render the 'old' sized framebuffer onto the 'new' sized ImGuiPanel and store the 'new' size in m_ViewportSize.
		// The next frame will first resize the framebuffer as m_ViewportSize differs from m_Framebuffer.Width / Height before updating and rendering.
		// This results in never rendering an empty(black) framebuffer.

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
}
