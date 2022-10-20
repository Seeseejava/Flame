#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




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

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.f / 720.f, true)
{
}

void Sandbox2D::OnAttach()
{
	FLAME_PROFILE_FUNCTION();

	m_CheckerboardTexture = Flame::Texture2D::Create("assets/texture/Checkerboard.png");
	m_SpriteSheet = Flame::Texture2D::Create("assets/RPGGame/texture/RPGpack_sheet_2X.png");

	Flame::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Flame::Framebuffer::Create(fbSpec);


	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
	s_TextureMap['D'] = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6 , 11}, {128, 128});
	s_TextureMap['W'] = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11 , 11}, {128, 128});
	s_TextureMap['S'] = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, {1 , 11}, {128, 128});

	m_TextureStair = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
	m_TextureGrass = Flame::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 3 }, { 128, 128 });
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
		m_Framebuffer->Bind();
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

		for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				char tileType = s_MapTiles[x + y * m_MapWidth];
				Flame::Ref<Flame::SubTexture2D> texture;

				if (s_TextureMap.find(tileType) != s_TextureMap.end())
					texture = s_TextureMap[tileType];
				else
					texture = m_TextureGrass;

				Flame::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, y - m_MapHeight / 2.0f, 0.5f }, { 1.0f,1.0f }, texture);
			}
		}

		/*Flame::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureGrass);
		Flame::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStair);
		Flame::Renderer2D::DrawQuad({ 2.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);*/
		Flame::Renderer2D::EndScene();

		m_Framebuffer->Unbind();
	}

	if (Flame::Input::IsMouseButtonPressed(FLAME_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Flame::Input::GetMousePosition();						//这个位置是相对于窗口左上角，且向右向下为正，单位为像素
		auto width = Flame::Application::Get().GetWindow().GetWidth();
		auto height = Flame::Application::Get().GetWindow().GetHeight();	//串口大小：1280x720

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
	
}

void Sandbox2D::OnImGuiRender()
{
	FLAME_PROFILE_FUNCTION();
	static bool dockingEnabled = true;
	if (dockingEnabled)
	{
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

				if (ImGui::MenuItem("Exit")) Flame::Application::Get().Close();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}


		ImGui::Begin("Settings");

		auto stats = Flame::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		//这里为了支持跨平台，需要把textureID从原本的GLuint改成void*类型，注意它是直接取整型数作为一个指针，这个指针当然是无效的，而不是取GLuint的地址改成void*
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();


		// 应该是ImGui对于贴图的Y坐标的认知跟目前的Texture是相反的
		//ImGui::Image((void*)textureID, ImVec2{ 1080.0f,720.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::Image((void*)textureID, ImVec2{ 1080.0f,720.0f });//采用了FrameBuffer之后就不用翻转了
		ImGui::End();

		ImGui::End();
	}
	else
	{
		ImGui::Begin("Settings");
		auto stats = Flame::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

		uint32_t textureID = m_CheckerboardTexture->GetRendererID();
		ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });
		ImGui::Image((void*)textureID, ImVec2{ 1280, 720 });
		ImGui::End();
	}
}

void Sandbox2D::OnEvent(Flame::Event& e)
{
	m_CameraController.OnEvent(e);
}

