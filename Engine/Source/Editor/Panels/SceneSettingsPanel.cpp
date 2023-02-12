#include "Editor/Panels/SceneSettingsPanel.h"
#include "Editor/IconManager/IconManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Renderer/Renderer3D.h"

#include <imgui/imgui.h>
#include <regex>

namespace Flame 
{
    SceneSettingsPanel::SceneSettingsPanel()
    {
		Ref<Texture> m_DefaultTexture = IconManager::GetInstance().GetNullTexture();

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

        // Still some bugs (top bottom inverse?), see https://stackoverflow.com/questions/55558241/opengl-cubemap-face-order-sampling-issue

		const auto& SkyBoxTreeNode = [&m_Paths = m_Paths](const char* nodeName, Ref<Texture>& tex, uint32_t pathIndex) {
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
						Renderer3D::GetSkyBox()->SetFace((FaceTarget)pathIndex, relativePath);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::TreePop();
			}
		};

		//Sky Box
		if (ImGui::TreeNode("SkyBox"))
		{
			if (ImGui::Button("Update"))
			{
				Renderer3D::GetSkyBox()->Generate();
			}

			SkyBoxTreeNode("Right +X", m_Right, 0);
			SkyBoxTreeNode("Left -X", m_Left, 1);
			SkyBoxTreeNode("Top +Y", m_Top, 2);
			SkyBoxTreeNode("Bottom -Y", m_Bottom, 3);
			SkyBoxTreeNode("Front +Z", m_Front, 4);
			SkyBoxTreeNode("Back -Z", m_Back, 5);

			ImGui::TreePop();
		}
		
		ImGui::End();
    }
}