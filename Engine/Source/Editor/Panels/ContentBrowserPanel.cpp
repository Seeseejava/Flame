#include "flamepch.h"
#include "ContentBrowserPanel.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include <imgui/imgui.h>

namespace Flame {

	namespace Utils
	{

	static bool HaveDirectoryMember(std::filesystem::path currentPath)
	{
		for (auto& directoryEntry : std::filesystem::directory_iterator(currentPath))
		{
			if (directoryEntry.is_directory())
				return true;
		}
		return false;
	}
	}

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(ConfigManager::GetInstance().GetAssetsFolder())
	{
		m_DirectoryIcon = Texture2D::Create(AssetManager::GetInstance().GetFullPath("Resources/ContentBrowser/DirectoryIcon.png").string());
		m_FileIcon = Texture2D::Create(AssetManager::GetInstance().GetFullPath("Resources/ContentBrowser/FileIcon.png").string());
	}

	void ContentBrowserPanel::OnImGuiRender(bool* pOpen)
	{


		// from imgui_demo.cpp: ShowAboutWindow function, should we add flag ImGuiWindowFlags_AlwaysAutoResize?
		if (!ImGui::Begin("Content Browser", pOpen))
		{
			ImGui::End();
			return;
		}

		ImGui::Columns(2);

		static bool init = true;
		if (init)
		{
			ImGui::SetColumnWidth(0, 200.0f);
			init = false;
		}

		if (ImGui::BeginChild("CONTENT_BROWSER_TREE"))
		{
			DrawTree();
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		ImGui::Separator();

		if (ImGui::BeginChild("CONTENT_BROWSER_CONTENT"))
		{
			DrawContent();
		}
		ImGui::EndChild();

		ImGui::Columns(1);

		ImGui::End();

	}

	void ContentBrowserPanel::DrawTree()
	{
		DrawTreeRecursive(ConfigManager::GetInstance().GetAssetsFolder());
	}

	void ContentBrowserPanel::DrawTreeRecursive(const std::filesystem::path& currentPath)
	{
		const ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

		ImGuiTreeNodeFlags nodeFlags = baseFlags;

		if (m_SelectedDirectory && *m_SelectedDirectory == currentPath)
		{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		bool bNeedOpen = true;
		if (!Utils::HaveDirectoryMember(currentPath))
		{
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			bNeedOpen = false;
		}

		bool nodeOpen = ImGui::TreeNodeEx(currentPath.filename().string().c_str(), nodeFlags);

		if (ImGui::IsItemClicked())
		{
			m_SelectedDirectory = currentPath;
		}

		if (nodeOpen && bNeedOpen)
		{
			for (auto p : std::filesystem::directory_iterator(currentPath))
			{
				auto path = p.path();
				if (!std::filesystem::is_directory(path))
				{
					continue;
				}

				DrawTreeRecursive(path);
			}
			ImGui::TreePop();
		}
	}

	void ContentBrowserPanel::DrawContent()
	{
		if (!m_SelectedDirectory)
		{
			return;
		}

		m_CurrentDirectory = *m_SelectedDirectory;

		static float padding = 8.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			ImGui::BeginGroup();

			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, ConfigManager::GetInstance().GetAssetsFolder());
			std::string filenameString = relativePath.filename().string();   //去掉filename就是相对路径的整个路径了

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t)); //这里加1是因为以NULL结尾
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
					m_SelectedDirectory = m_CurrentDirectory;
				}
			}

			ImVec2 text_size = ImGui::CalcTextSize(filenameString.c_str());
			ImVec2 pos = ImGui::GetCursorPos();
			pos.x += (thumbnailSize - text_size.x) * 0.5f;
			ImGui::SetCursorPos(pos);

			ImGui::TextWrapped(filenameString.c_str());  //auto-resizing 

			ImGui::EndGroup();

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);

	}

}