#include "flamepch.h"
#include "ContentBrowserPanel.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Editor/IconManager/IconManager.h"
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

		static bool IsImageFormat(std::string filePath)
		{
			std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
			if (extension == "png" || extension == "jpg" || extension == "bmp" || extension == "hdr" || extension == "tga")
			{
				return true;
			}
			return false;
		}

		static bool IsMusicFormat(std::string filePath)
		{
			std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
			if (extension == "mp3" || extension == "wma" || extension == "wav")
			{
				return true;
			}
			return false;
		}

	}

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(ConfigManager::GetInstance().GetAssetsFolder())
	{
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
			ImGui::SetColumnWidth(0, 240.0f);
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

		std::string label = "##" + currentPath.filename().string();
		bool nodeOpen = ImGui::TreeNodeEx(label.c_str(), nodeFlags);

		if (ImGui::IsItemClicked())
		{
			m_SelectedDirectory = currentPath;
		}

		ImGui::SameLine();
		ImGui::Image((ImTextureID)IconManager::GetInstance().GetDirectoryIcon()->GetRendererID(), { 20.0f, 20.0f }, { 0, 1 }, { 1, 0 });
		ImGui::SameLine();
		ImGui::Text(currentPath.filename().string().c_str());

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

		std::vector<std::filesystem::path> sortedDirectory;
		int directoryEndIndex = -1;

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			if (!directoryEntry.is_directory())
			{
				sortedDirectory.push_back(directoryEntry.path());
			}
			else
			{
				sortedDirectory.insert(sortedDirectory.begin(), directoryEntry.path());
				directoryEndIndex++;
			}
		}

		for (int i = 0; i < sortedDirectory.size(); i++)
		{
			ImGui::BeginGroup();

			const auto& path = sortedDirectory[i];
			auto relativePath = std::filesystem::relative(path, ConfigManager::GetInstance().GetAssetsFolder());
			std::string filenameString = relativePath.filename().string();   //去掉filename就是相对路径的整个路径了

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = i <= directoryEndIndex ? IconManager::GetInstance().GetDirectoryIcon() : IconManager::GetInstance().GetFileIcon();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			if (Utils::IsImageFormat(path.string()))
			{
				std::string texturePath = "Asset\\" + relativePath.string();
				Ref<Texture2D> img = IconManager::GetInstance().LoadOrFindTexture(texturePath);
				ImGui::ImageButton((ImTextureID)img->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			}
			else if (Utils::IsMusicFormat(path.string()))
			{
				ImGui::ImageButton((ImTextureID)IconManager::GetInstance().Get("MusicIcon")->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			}
			else
			{
				ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			}

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t)); //这里加1是因为以NULL结尾
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (i <= directoryEndIndex)
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
	}
}