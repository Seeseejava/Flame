#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace Flame {
	static const uint32_t s_MapWidth = 16;
	static const char s_MapTiles[] =
	{
		// 这种写法其实代表一个长字符串, D代表Dirt土地Tile, W代表Water Tile, S代表路标Tile
		// 注意第一个Tile为D, 虽然在数组里坐标为(0,0), 但是在屏幕上对应的坐标应该是(0,1)
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
		// 改变Viewport窗口大小，竖直方向的区域会随着缩放，但竖直方向的视野不变；水平方向的区域也会缩放，但水平方向的区域视野也会变化
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
			m_CameraController.OnUpdate(ts); //更新键盘事件

		// Render
		Renderer2D::ResetStats();
		{
			FLAME_PROFILE_SCOPE("Renderer Prep");
			//清除默认frambuffer
			Flame::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));// 默认窗口颜色仍为magenta
			Flame::RenderCommand::Clear();
			m_Framebuffer->Bind();
			// clear我的Framebuffer
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();
		}

		{
			FLAME_PROFILE_SCOPE("Renderer Draw");

			// 地图
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

			//		Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight / 2.0f - y, 0.1f }, { 1.0f,1.0f }, texture);// 这里需要翻转一下y轴
			//	}
			//}

			/*Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureGrass);
			Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStair);
			Renderer2D::DrawQuad({ 2.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);*/


			m_ActiveScene->OnUpdate(ts);


		}

		if (Input::IsMouseButtonPressed(FLAME_MOUSE_BUTTON_LEFT))
		{
			auto [x, y] = Input::GetMousePosition();						//这个位置是相对于窗口左上角，且向右向下为正，单位为像素
			auto width = Application::Get().GetWindow().GetWidth();
			auto height = Application::Get().GetWindow().GetHeight();	//串口大小：1280x720

			auto bounds = m_CameraController.GetBounds();						//当zoomlevel时，left = -5 * 1280 / 720, bottom = -5;
			auto pos = m_CameraController.GetCamera().GetPosition();			//当摄像机未移动时为0
			x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;		//前一部分求得相较于摄像机左下角的位置，减摄像机半宽后得到相对于摄像机的位置（变换成摄像机的坐标了）
			y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
			m_Particle.Position = { x + pos.x, y + pos.y };
			for (int i = 0; i < 5; i++)
				m_ParticleSystem.Emit(m_Particle);
		}

		m_ParticleSystem.OnUpdate(ts);
		m_ParticleSystem.OnRender(m_CameraController.GetCamera());
		m_Framebuffer->Unbind(); //目前粒子系统的鼠标位置有问题

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
			glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3])); // mat4中4行4列的元素是按照列的方式排列的:主要是因为opengl在显存中是按列存储的

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

		//设置窗口的padding为0是图片控件充满窗口
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();//窗口尺寸
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
