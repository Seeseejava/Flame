#pragma once
#include "Flame.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneSettingsPanel.h"
#include "Log/OutputLog.h"

#include "Runtime/Camera/EditorCamera.h"
#include "Runtime/Renderer/RenderPass.h"

#include <spdlog/sinks/base_sink.h>
namespace Flame {

	class EditorLayer : public Layer
	{
	private:
		template<typename Mutex>
		class my_sink : public spdlog::sinks::base_sink <Mutex>
		{
		protected:
			void sink_it_(const spdlog::details::log_msg& msg) override
			{
				spdlog::memory_buf_t formatted;
				spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
				m_OutputLog.AddLog(fmt::to_string(formatted).c_str());
			}

			void flush_() override
			{
				std::cout << std::flush;
			}
		};
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		static void EditorLogInit();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void UpdateSettings();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();

		void LoadDefaultEditorConfig();
	private:

		// Temp
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;
		Ref<RenderPass> m_RenderPass;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		Ref<Texture2D> m_CheckerboardTexture, m_SpriteSheet, m_FaceTexture;
		Ref<SubTexture2D> m_TextureStair, m_TextureGrass, m_TextureTree;


		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };


		uint32_t m_MapHeight, m_MapWidth;
		std::unordered_map<char, Ref<SubTexture2D>> s_TextureMap;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_LastViewportSize = { 0 , 0 };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		SceneSettingsPanel m_SceneSettingsPanel;

		static OutputLog m_OutputLog;

	};
}