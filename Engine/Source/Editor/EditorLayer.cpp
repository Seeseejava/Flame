#include "EditorLayer.h"
#include "Editor/ImGuiWrapper/ImGuiWrapper.h"
#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include "Editor/IconManager/IconManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Runtime/ECS/System/Physics/PhysicsSystem3D.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/ECS/Serializer/SceneSerializer.h"
#include "Runtime/Utils/PlatformUtils.h"
#include "Runtime/Math/Math.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include <magic_enum.hpp>

#include "ImGuizmo.h"



namespace Flame {

	namespace Utils
	{
		// from https://github.com/Acmen-Team/Epoch/tree/dev
		template<typename UIFunction>
		static void SceneToolbar(ImGuiDockNode* node, const float DISTANCE, int* corner, UIFunction uiFunc)
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

			ImVec2 work_area_pos = node->Pos;
			ImVec2 work_area_size = node->Size;

			if (*corner != -1)
			{
				window_flags |= ImGuiWindowFlags_NoMove;
				ImGuiViewport* viewport = ImGui::GetMainViewport();

				ImVec2 window_pos = ImVec2((*corner & 1) ? (work_area_pos.x + work_area_size.x - DISTANCE) : (work_area_pos.x + DISTANCE), (*corner & 2) ? (work_area_pos.y + work_area_size.y - DISTANCE) : (work_area_pos.y + DISTANCE));
				ImVec2 window_pos_pivot = ImVec2((*corner & 1) ? 1.0f : 0.0f, (*corner & 2) ? 1.0f : 0.0f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
				ImGui::SetNextWindowViewport(node->ID);
			}
			ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background

			uiFunc(corner, work_area_size, window_flags);
		}
	}

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


	extern const std::filesystem::path g_AssetPath;

	// Window
	static bool bShowViewport = true;
	static bool bShowContentBrowser = true;
	static bool bShowSceneHierachy = true;
	static bool bShowProperties = true;
	static bool bShowStats = false;
	static bool bShowEngineSettings = true;
	static bool bShowSceneSettings = true;
	static bool bShowSRT = true;
	static bool bShowOutputLog = true;


	// Help
	static bool bShowTutorial = false;
	static bool bShowDemoImGui = false;
	static bool bShowAboutMe = false;

	OutputLog EditorLayer::m_OutputLog;

	EditorLayer::EditorLayer()
		:Layer("EditorLayer")
	{
	}

	void EditorLayer::EditorLogInit()
	{
		using my_sink_st = my_sink<spdlog::details::null_mutex>;
		Log::GetCoreLogger() = CreateRef<spdlog::logger>("HEngine", CreateRef<my_sink_st>());
		Log::GetCoreLogger()->set_level(spdlog::level::trace);
	}

	void EditorLayer::OnAttach()
	{
		FLAME_PROFILE_FUNCTION();

		ImGuiContext& g = *GImGui;
#define IMGUI_INI_FILE FLAME_XSTRING(ENGINE_ROOT_DIR) "/Config/EditorLayout.ini"
		g.IO.IniFilename = IMGUI_INI_FILE;

		m_CheckerboardTexture = Texture2D::Create(AssetManager::GetFullPath("Assets/texture/Checkerboard.png"));
		m_SpriteSheet = Texture2D::Create(AssetManager::GetFullPath("Assets/RPGGame/texture/RPGpack_sheet_2X.png"));

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		fbSpec.Samples = 4;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		RenderPassSpecification rpSpec = { m_Framebuffer, "MainPass" };
		m_RenderPass = std::make_shared<RenderPass>(rpSpec);
		m_RenderPass->AddPostProcessing(PostProcessingType::MSAA); //default


		m_ActiveScene = std::make_shared<Scene>();

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);


		m_MapWidth = s_MapWidth;
		m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
		s_TextureMap['D'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6 , 11 }, { 128, 128 });
		s_TextureMap['W'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11 , 11 }, { 128, 128 });
		s_TextureMap['S'] = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1 , 11 }, { 128, 128 });

		m_TextureStair = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
		m_TextureGrass = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 3 }, { 128, 128 });
		m_TextureTree = SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2 , 1 }, { 128, 128 }, { 1, 2 });

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
		if (ConfigManager::m_ViewportSize.x > 0.0f && ConfigManager::m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != ConfigManager::m_ViewportSize.x || spec.Height != ConfigManager::m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);

			m_EditorCamera.SetViewportSize(ConfigManager::m_ViewportSize.x, ConfigManager::m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);
			PostProcessing::m_Framebuffer->Resize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);
		}


		// Render
		Renderer2D::ResetStats();
		{
			FLAME_PROFILE_SCOPE("Renderer Prep");
			//清除默认frambuffer
			Flame::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));// 默认窗口颜色仍为magenta(这一句好像没用）
			Flame::RenderCommand::Clear();
			m_Framebuffer->Bind();
			// clear我的Framebuffer
			RenderCommand::SetClearColor({ 0.4f, 0.4f, 0.4f, 1 });
			//RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			m_Framebuffer->ClearAttachment(1, -1);
		}

		{
			FLAME_PROFILE_SCOPE("Renderer Draw");


			if (ModeManager::IsEditState())
			{

				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

			}
			else
			{
				m_ActiveScene->OnUpdateRuntime(ts);
			}

			auto [mx, my] = ImGui::GetMousePos(); //相对于屏幕的坐标
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;
			glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			my = viewportSize.y - my;
			int mouseX = (int)mx;
			int mouseY = (int)my;

			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
			{
				//FLAME_CORE_WARN("{0}, {1}", mouseX, mouseY);
				int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
				//FLAME_CORE_WARN("Pixel data = {0}", pixelData);
				m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
			}

		}

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			ConfigManager::selectedEntity = (int)(uint32_t)selectedEntity;
			if (Input::IsKeyPressed(FLAME_KEY_F))
				m_EditorCamera.SetCenter(selectedEntity.GetComponent<TransformComponent>().Translation);
		}
		else
		{
			ConfigManager::selectedEntity = -1;
		}


		m_Framebuffer->Unbind(); 

	}

	void EditorLayer::OnImGuiRender()
	{
		FLAME_PROFILE_FUNCTION();

		UpdateSettings();

		static bool bChangeDim = false;

		// ----DockSpace Begin----
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
		//-----设置docking栏的最小宽度
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 200.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		// ----MenuBar Begin----
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{


				// Disabling full screen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit")) 
					Application::GetInstance().Close();

				ImGui::EndMenu();
			}
				
			if (ImGui::BeginMenu("Window"))
			{
				const float lineSize = 300.0f;
				const float separatorTextGray = 0.5f;
				auto SeparatorWithText = [&lineSize, &separatorTextGray](const char* text) {
					ImGui::TextColored(ImColor(separatorTextGray, separatorTextGray, separatorTextGray, 1.0f), text);
					ImGui::SameLine();
					ImGuiWrapper::CenteredSeparator(lineSize - ImGui::CalcTextSize(text).x);
				};
				SeparatorWithText("PANEL  ");

				ImGui::MenuItem("Viewport", NULL, &bShowViewport);
				ImGui::MenuItem("Content Browser", NULL, &bShowContentBrowser);
				ImGui::MenuItem("Scene Hierachy", NULL, &bShowSceneHierachy);
				ImGui::MenuItem("Properties", NULL, &bShowProperties);
				ImGui::MenuItem("Stats", NULL, &bShowStats);
				ImGui::MenuItem("Engine Settings", NULL, &bShowEngineSettings);
				ImGui::MenuItem("Environment Settings", NULL, &bShowSceneSettings);

				SeparatorWithText("LOG  ");

				ImGui::MenuItem("Output Log", NULL, &bShowOutputLog);

				SeparatorWithText("LAYOUT  ");

				if (ImGui::MenuItem("Load Default Layout"))
				{
					LoadDefaultEditorConfig();
					ImGui::EndMenu();
					ImGui::EndMenuBar();
					ImGui::End();
					return;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("Tutorial", NULL, &bShowTutorial);
				ImGui::MenuItem("About Me", NULL, &bShowAboutMe);
				ImGui::MenuItem("Demo ImGui", NULL, &bShowDemoImGui);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		// ----MenuBar End----

		// ----Windows Begin----
		if (bShowContentBrowser)
		{
			m_ContentBrowserPanel.OnImGuiRender(&bShowContentBrowser);
		}
		if (bShowSceneHierachy || bShowProperties)
		{
			m_SceneHierarchyPanel.OnImGuiRender(&bShowSceneHierachy, &bShowProperties);
		}

		if (bShowStats)
		{
			ImGui::Begin("Stats", &bShowStats);

			std::string name = "None";
			if (m_HoveredEntity)
				name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("Hovered Entity: %s", name.c_str());

			auto stats = Renderer2D::GetStats();
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quads: %d", stats.QuadCount);
			ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
			ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

			ImGui::End();
		}

		if (bShowEngineSettings)
		{
			ImGui::Begin("Engine Setting", &bShowEngineSettings);


			const char* modes[] = { "2D", "3D" };
			int lastMode = ModeManager::b3DMode;

			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);

			ImGui::Text("Mode");
			ImGui::NextColumn();
			if (ImGui::Combo("##Mode", &ModeManager::b3DMode, modes, IM_ARRAYSIZE(modes)))
			{
				if (lastMode != ModeManager::b3DMode)
				{
					bChangeDim = true;
				}
			}
			ImGui::EndColumns();

			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnWidth(0, 100.0f);
			ImGui::Text("Camera Speed");
			ImGui::NextColumn();
			ImGui::SliderFloat("##Camera Speed", &m_EditorCamera.m_CameraSpeed, 0.1f, 100.0f);
			ImGui::EndColumns();

			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Physics Settings", "Physics Settings"))
			{
				ImGui::Checkbox("Show physics colliders", &ModeManager::bShowPhysicsColliders);

				if (ImGui::BeginMenu("Show physics colliders modes"))
				{
					using namespace magic_enum::bitwise_operators;

					constexpr auto flags = magic_enum::enum_values<PhysicsDebugDrawModeFlag>();
					for (auto flag : flags)
					{
						if (ImGui::MenuItem(magic_enum::enum_name(flag).data(), NULL, (bool)(ModeManager::m_PhysicsDebugDrawModeFlag & flag)))
						{
							(bool)(ModeManager::m_PhysicsDebugDrawModeFlag & flag) ?
								ModeManager::m_PhysicsDebugDrawModeFlag &= ~flag : ModeManager::m_PhysicsDebugDrawModeFlag |= flag;
							PhysicsSystem3D::SetDebugMode((int)ModeManager::m_PhysicsDebugDrawModeFlag);
						}
					}

					ImGui::EndMenu();
				}
				ImGui::TreePop();
			}

			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Post Processing", "Post Processing"))
			{
				if (ImGui::Button("Add Post Processing"))
					ImGui::OpenPopup("AddPostProcessing");

				if (ImGui::BeginPopup("AddPostProcessing"))
				{
					constexpr auto postTypes = magic_enum::enum_values<PostProcessingType>();

					// Skip None and MSAA
					for (size_t i = 2; i < postTypes.size(); i++)
					{
						if (ImGui::MenuItem(magic_enum::enum_name(postTypes[i]).data()))
						{
							m_RenderPass->AddPostProcessing(postTypes[i]);
							ImGui::CloseCurrentPopup();
						}
						
					}

					ImGui::EndPopup();
				}

				for (size_t i = 1; i < m_RenderPass->m_PostProcessings.size(); i++)
				{
					ImGui::Selectable(magic_enum::enum_name(m_RenderPass->m_PostProcessings[i]->m_Type).data());

					// imgui demo: Drag to reorder items (simple)
					if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
					{
						int next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
						if (next >= 1 && next < m_RenderPass->m_PostProcessings.size())
						{
							PostProcessingType tempType = m_RenderPass->m_PostProcessings[i]->m_Type;
							m_RenderPass->m_PostProcessings[i]->m_Type = m_RenderPass->m_PostProcessings[next]->m_Type;
							m_RenderPass->m_PostProcessings[next]->m_Type = tempType;
							ImGui::ResetMouseDragDelta();
						}
					}


					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
							m_RenderPass->m_PostProcessings.erase(m_RenderPass->m_PostProcessings.begin() + i);

						ImGui::EndPopup();
					}
				}

				ImGui::TreePop();
			}
			ImGui::End();
		}

		if (bShowSceneSettings)
		{
			m_SceneSettingsPanel.OnImGuiRender(&bShowSceneSettings);

			ImGui::Begin("Scene Settings", &bShowSceneSettings);

			ImGui::End();
		}

		if (bShowOutputLog)
		{
			m_OutputLog.OnImGuiRender(&bShowOutputLog);
		}

		if (bShowViewport)
		{
			//设置窗口的padding为0是图片控件充满窗口
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar (获得viewport有无bar后显示区域的坐标）：无bar（0，0），有bar(0.24) //注意于与ImGui::GetMousePos()的区别

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();// viewport显示区域大小，不包括tab bar
			//m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y }; // 没有这句viewport会是黑的 -> 说明图片大小必须和viewport大小相同才不会是黑的
			ConfigManager::m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			m_ViewportFocused = ImGui::IsWindowFocused();
			m_ViewportHovered = ImGui::IsWindowHovered();
			//Application::GetInstance().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
			Application::GetInstance().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

			// This solution will render the 'old' sized framebuffer onto the 'new' sized ImGuiPanel and store the 'new' size in m_ViewportSize.
			// The next frame will first resize the framebuffer as m_ViewportSize differs from m_Framebuffer.Width / Height before updating and rendering.
			// This results in never rendering an empty(black) framebuffer.

			uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0); // 按索引显示哪一个colorbuffer
			textureID = m_RenderPass->ExcuteAndReturnFinalTex();
			ImGui::Image((void*)(intptr_t)textureID, ImVec2{ ConfigManager::m_ViewportSize.x, ConfigManager::m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			auto windowSize = ImGui::GetWindowSize(); // 整个viewport大小，包括tab bar
			ImVec2 minBound = ImGui::GetWindowPos(); // 整个viewport相对于屏幕左上角的位置
			//FLAME_CORE_WARN("{0}, {1}", viewportPanelSize.x, viewportPanelSize.y);
			//FLAME_CORE_WARN("{0}, {1}", windowSize.x, windowSize.y);
			//FLAME_CORE_WARN("{0}, {1}", minBound.x, minBound.y);
			minBound.x += viewportOffset.x;
			minBound.y += viewportOffset.y;// 绘制区域在屏幕上的坐标

			ImVec2 maxBound = { minBound.x + ConfigManager::m_ViewportSize.x, minBound.y + ConfigManager::m_ViewportSize.y };
			m_ViewportBounds[0] = { minBound.x, minBound.y };
			m_ViewportBounds[1] = { maxBound.x, maxBound.y };

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					OpenScene(std::filesystem::path(ConfigManager::GetInstance().GetAssetsFolder()) / path);
				}
				ImGui::EndDragDropTarget();
			}

			// Gizmos
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			if (selectedEntity && m_GizmoType != -1)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				// Runtime camera from entity
				/*auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
				const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
				const glm::mat4& cameraProjection = camera.GetProjection();
				glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());*/

				// Editor camera
				const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
				glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

				// Entity transform
				auto& tc = selectedEntity.GetComponent<TransformComponent>();
				glm::mat4 transform = tc.GetTransform();

				// Snapping
				bool snap = Input::IsKeyPressed(FLAME_KEY_LEFT_CONTROL);
				float snapValue = 0.5f; // Snap to 0.5m for translation/scale
				// Snap to 45 degrees for rotation
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					snapValue = 45.0f;

				float snapValues[3] = { snapValue, snapValue, snapValue };

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
					nullptr, snap ? snapValues : nullptr);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					Math::DecomposeTransform(transform, translation, rotation, scale);

					glm::vec3 deltaRotation = rotation - tc.Rotation;
					tc.Translation = translation;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}

			static int transCorner = 1;
			ImGuiDockNode* node = ImGui::GetWindowDockNode();
			node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

			Utils::SceneToolbar(node, 10.0f, &transCorner, [&](int* corner, const ImVec2& work_area_size, const ImGuiWindowFlags m_window_flags) {

				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

			if (ImGui::Begin("SRT Toolbar", &bShowSRT, m_window_flags))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.1f, 0.1f, 0.1f, 0.5f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

				if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("TransIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
				{
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("RotateIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
				{
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				}
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)IconManager::GetInstance().Get("ScaleIcon")->GetRendererID(), ImVec2(30.0f, 30.0f), ImVec2(0, 1), ImVec2(1, 0), 0.0f, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImVec4(1.0f, 1.0f, 0.0f, 0.9f) : ImVec4(1.0f, 1.0f, 0.0f, 0.2f)))
				{
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}

				ImGui::PopStyleColor(3);

			}

			ImGui::End();
			ImGui::PopStyleVar();
				});

			ImGui::End();
			ImGui::PopStyleVar();
		}
		// ----Windows End----
		// 
		// ----Help Begin----
		// TODO
		ImGuiWindowFlags helpMenuFlags = ImGuiWindowFlags_NoDocking;
		if (bShowTutorial)
		{
			ImGui::Begin("Tutorial", &bShowTutorial, helpMenuFlags);
			ImGui::Text("TODO: HELP LIST");

			ImGui::End();
		}
		if (bShowDemoImGui)
		{
			ImGui::ShowDemoWindow(&bShowDemoImGui);
		}
		if (bShowAboutMe)
		{
			ImGui::Begin("About Me", &bShowAboutMe, helpMenuFlags);
			ImGui::Text("This project is learned from Cherno!");
			ImGui::End();
		}
		// ----Help End----

		UI_Toolbar();

		ImGui::End();
		// ----DockSpace End----

		if (bChangeDim)
		{
			m_ActiveScene->ChangeDimMode();
			bChangeDim = false;
		}

	}

	void EditorLayer::LoadDefaultEditorConfig()
	{
		const std::filesystem::path DefaultEditorConfigPath{ AssetManager::GetFullPath("Config/EditorLayoutDefault.ini") };
		FLAME_CORE_ASSERT(std::filesystem::exists(DefaultEditorConfigPath), "No imgui.ini");
		ImGui::LoadIniSettingsFromDisk(DefaultEditorConfigPath.string().c_str());


		// TODO:fix the bug
		std::filesystem::copy(DefaultEditorConfigPath, std::filesystem::current_path());

		// Window
		bShowViewport = true;
		bShowContentBrowser = true;
		bShowSceneHierachy = true;
		bShowProperties = true;
		bShowStats = false;
		bShowEngineSettings = true;
		bShowSceneSettings = true;

		// Help
		bShowTutorial = false;
		bShowAboutMe = false;
		bShowDemoImGui = false;

	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGuiDockNode* node = ImGui::GetWindowDockNode();
		node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = ModeManager::IsEditState() ? IconManager::GetInstance().GetPlayIcon() : IconManager::GetInstance().GetStopIcon();
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (ModeManager::IsEditState())
				OnScenePlay();
			else 
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(FLAME_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(FLAME_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(FLAME_KEY_LEFT_CONTROL) || Input::IsKeyPressed(FLAME_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(FLAME_KEY_LEFT_SHIFT) || Input::IsKeyPressed(FLAME_KEY_RIGHT_SHIFT);
		bool mouseRight = Input::IsMouseButtonPressed(FLAME_MOUSE_BUTTON_RIGHT);
		switch (e.GetKeyCode())
		{
			case FLAME_KEY_N:
			{
				if (control)
					NewScene();

				break;
			}
			case FLAME_KEY_O:
			{
				if (control)
					OpenScene();

				break;
			}
			case FLAME_KEY_S:
			{
				if (control && shift)
					SaveSceneAs();
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
				break;
			}

			// Scene Commands
			case FLAME_KEY_D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}
			// Gizmos
			case FLAME_KEY_Q:
				if (!mouseRight)
					m_GizmoType = -1;
				break;
			case FLAME_KEY_W:
				if (!mouseRight)
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case FLAME_KEY_E:
				if (!mouseRight)
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case FLAME_KEY_R:
				if (!mouseRight)
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}		
	}



	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == FLAME_MOUSE_BUTTON_LEFT)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(FLAME_KEY_LEFT_ALT))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::UpdateSettings()
	{
		// from imgui.cpp
		// Load settings on first frame (if not explicitly loaded manually before)
		ImGuiContext& g = *GImGui;
		if (!g.SettingsLoaded)
		{
			IM_ASSERT(g.SettingsWindows.empty());
			if (g.IO.IniFilename)
				ImGui::LoadIniSettingsFromDisk(g.IO.IniFilename);
			g.SettingsLoaded = true;
		}

		// Save settings (with a delay after the last modification, so we don't spam disk too much)
		if (g.SettingsDirtyTimer > 0.0f)
		{
			g.SettingsDirtyTimer -= g.IO.DeltaTime;
			if (g.SettingsDirtyTimer <= 0.0f)
			{
				if (g.IO.IniFilename != NULL)
					ImGui::SaveIniSettingsToDisk(g.IO.IniFilename);
				else
					g.IO.WantSaveIniSettings = true;  // Let user know they can call SaveIniSettingsToMemory(). user will need to clear io.WantSaveIniSettings themselves.
				g.SettingsDirtyTimer = 0.0f;
			}
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = std::make_shared<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		// 前面的Flame Scene(*.flame)是展示在filter里的text, 后面的*.flame代表显示的文件后缀类型
		std::string filepath = FileDialogs::OpenFile("Flame Scene (*.flame)\0*.flame\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (!ModeManager::IsEditState())
		{
			OnSceneStop();
			ModeManager::ChangeState();
		}

		if (path.extension().string() != ".flame")
		{
			FLAME_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = std::make_shared<Scene>();
		newScene->OnViewportResize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y); //至关重要，hazelnut里面没有
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)ConfigManager::m_ViewportSize.x, (uint32_t)ConfigManager::m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Flame Scene (*.flame)\0*.flame\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		ModeManager::ChangeState();

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		ModeManager::ChangeState();

		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (!ModeManager::IsEditState())
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}
}
