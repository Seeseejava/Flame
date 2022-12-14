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

				Flame::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight / 2.0f - y, 0.5f }, { 1.0f,1.0f }, texture);
			}
		}

		/*Flame::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureGrass);
		Flame::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStair);
		Flame::Renderer2D::DrawQuad({ 2.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);*/
		Flame::Renderer2D::EndScene();
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
	
	ImGui::Begin("Settings");
	auto stats = Flame::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	uint32_t textureID = m_SpriteSheet->GetRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });
	ImGui::End();

}

void Sandbox2D::OnEvent(Flame::Event& e)
{
	m_CameraController.OnEvent(e);
}

