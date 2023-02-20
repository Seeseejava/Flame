#include "Editor/Panels/SceneSettingsPanel.h"
#include "Editor/IconManager/IconManager.h"
#include "Editor/ImGuiWrapper.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Renderer/Renderer3D.h"
#include "Runtime/Library/TextureLibrary.h"
#include "Runtime/ECS/System/Render/EnvironmentSystem.h"

#include <imgui/imgui.h>
#include <regex>
#include <filesystem>

namespace Flame 
{
    SceneSettingsPanel::SceneSettingsPanel()
    {
		Ref<Texture2D> m_DefaultTexture = IconManager::GetInstance().GetNullTexture();

        m_Paths = std::vector<std::string>(6);
        m_Right  = m_DefaultTexture;
        m_Left   = m_DefaultTexture;
        m_Top    = m_DefaultTexture;
        m_Bottom = m_DefaultTexture;
        m_Front  = m_DefaultTexture;
        m_Back   = m_DefaultTexture;
    }

	void SceneSettingsPanel::OnImGuiRender(bool* pOpen)
	{
		if (!ImGui::Begin("Scene Settings", pOpen))
		{
			ImGui::End();
			return;
		}

		const char* modes[] = { "None", "Environment Hdr", "Sky Box" };
		ImGui::Text("Mode");
		ImGui::SameLine();
		if (ImGui::Combo("##Mode", (int*)(&ModeManager::m_SceneMode), modes, IM_ARRAYSIZE(modes)))
		{

		}

		// Still some bugs (top bottom inverse?), see https://stackoverflow.com/questions/55558241/opengl-cubemap-face-order-sampling-issue

		const auto& SkyBoxTreeNode = [&m_Paths = m_Paths](const char* nodeName, Ref<Texture2D>& tex, uint32_t pathIndex) {
			if (ImGui::TreeNode(nodeName))
			{
				ImGui::Image((ImTextureID)tex->GetRendererID(), ImVec2(64, 64), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						auto path = (const wchar_t*)payload->Data;
						std::string relativePath = (std::filesystem::path("Assets") / path).string();
						std::filesystem::path texturePath = ConfigManager::GetInstance().GetAssetsFolder() / path;
						m_Paths[pathIndex] = texturePath.string();
						relativePath = std::regex_replace(relativePath, std::regex("\\\\"), "/");
						tex = IconManager::GetInstance().LoadOrFindTexture(relativePath);
						Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture")->SetFace((FaceTarget)pathIndex, relativePath);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TreePop();
			}
		};

		//Sky Box
		if (ModeManager::m_SceneMode == SceneMode::SkyBox)
		{
			if (ImGuiWrapper::TreeNodeExStyle1((void*)"SkyBox", "SkyBox"))
			{
				if (ImGui::Button("Update"))
				{
					Library<CubeMapTexture>::GetInstance().Get("SkyBoxTexture")->Generate();
				}

				SkyBoxTreeNode("Right +X", m_Right, 0);
				SkyBoxTreeNode("Left -X", m_Left, 1);
				SkyBoxTreeNode("Top +Y", m_Top, 2);
				SkyBoxTreeNode("Bottom -Y", m_Bottom, 3);
				SkyBoxTreeNode("Front +Z", m_Front, 4);
				SkyBoxTreeNode("Back -Z", m_Back, 5);

				ImGui::TreePop();
			}
		}
		else if (ModeManager::m_SceneMode == SceneMode::EnvironmentHdr)
		{
			if (ImGuiWrapper::TreeNodeExStyle1((void*)"Environment Hdr", "Environment Hdr"))
			{
				ImGui::Image((ImTextureID)Library<Texture2D>::GetInstance().Get("DefaultHdr")->GetRendererID(), ImVec2(64, 64), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						auto path = (const wchar_t*)payload->Data;
						std::string relativePath = (std::filesystem::path("Assets") / path).string();
						std::filesystem::path texturePath = ConfigManager::GetInstance().GetAssetsFolder() / path;
						relativePath = std::regex_replace(relativePath, std::regex("\\\\"), "/");
						Library<Texture2D>::GetInstance().Set("DefaultHdr", IconManager::GetInstance().LoadOrFindTexture(relativePath));
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				ImGui::Checkbox("Use", &ModeManager::bHdrUse);

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("SkyBox Lod");
				ImGui::NextColumn();
				ImGui::SliderFloat("##SkyBox Lod", &EnvironmentSystem::environmentSettings.SkyBoxLod, 0.0f, 10.0f);
				ImGui::EndColumns();

				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Exposure");
				ImGui::NextColumn();
				ImGui::SliderFloat("##Exposure", &EnvironmentSystem::environmentSettings.exposure, 0.0f, 10.0f);
				ImGui::EndColumns();


				ImGui::TreePop();
			}
		}
			ImGui::End();
	}
	
}