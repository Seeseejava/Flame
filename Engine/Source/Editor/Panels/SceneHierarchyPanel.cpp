#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "Editor/ImGuiWrapper.h"
#include "Editor/Panels/SceneHierarchyPanel.h"
#include "Editor/IconManager/IconManager.h"

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include "Runtime/Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Utils/PlatformUtils.h"

namespace Flame {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender(bool* pOpen, bool* pOpenProperties)
	{
		if (*pOpen)
		{
			ImGui::Begin("Scene Hierarchy");

			if (m_Context)
			{
				m_Context->m_Registry.each([&](auto entityID)
					{
						Entity entity{ entityID , m_Context.get() };
						DrawEntityNode(entity);
					});

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
					m_SelectionContext = {};

				// Right-click on blank space
				// Right-click on blank space
				// 1��������Ҽ�(0���������2�����м�), bool over_itemΪfalse, ��ζ���������ֻ�ڿհ״�����Żᴥ�� 
				// ����Ӧ��������item�ϵ��, �޷Ǵ�ʱ����������GameObject
				if (ImGui::BeginPopupContextWindow(0, 1, false))
				{
					if (ImGui::MenuItem("Create Empty Entity"))
						m_Context->CreateEntity("Empty Entity");

					if (ImGui::MenuItem("Create Point Light"))
					{
						auto entity = m_Context->CreateEntity("Light");
						entity.AddComponent<LightComponent>();
						SetSelectedEntity(entity);
					}

					ImGui::EndPopup();
				}
			}

			ImGui::End();

			ImGui::Begin("Properties");
			if (m_SelectionContext)
			{
				DrawComponents(m_SelectionContext);
			}

			ImGui::End();
		}
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		const char* tag = "Unnamed Entity";
		if (entity.HasComponent<TagComponent>())
		{
			tag = entity.GetComponent<TagComponent>().Tag.c_str();
		}


		// ÿ��node���Դ�OpenOnArrow��flag, �����ǰentity�����Ǳ�ѡ���go, ��ô�����һ��selected flag
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // ���������ռ�
		// �����TreeNodeEx����ImGui���������HashCode(GUID), ����һ��TreeNode, ����������Ҫһ��
		// void*ָ��, ����ֱ�Ӱ�entity��idת��void*��������
		// exӦ����expanded����˼, �����ж�go��Ӧ��Node�Ƿ���չ��״̬
		std::string label = std::string("##") + std::string(tag);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, ""); //������Ϊָ����64λ�ģ�Ϊ�˳������⣬��ת����64λ

		// ������������item��, �ҵ����������, �򷵻�true
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(1.0f, 1.0f, 1.0f, 0.0f));
		ImGui::Image((ImTextureID)IconManager::GetInstance().Get("EntityIcon")->GetRendererID(), ImVec2{ lineHeight - 5.0f, lineHeight - 5.0f }, { 0, 1 }, { 1, 0 });
		ImGui::PopStyleColor(1);
		ImGui::SameLine();
		ImGui::Text(tag);

		// ����˽ڵ���expanded״̬, ��ô��Ҫ����loop������ȥ
		// ����Ŀǰû����ʽentity, ���������չ���Ķ����ٻ���һ����ͬ���ӽڵ�
		if (opened)
		{
			// TreePopò���Ǹ������Ĳ���, ����ÿ���ڵ���ƽ���ʱҪ���ô˺���
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}

	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[1];

		// Translation��Scale��������ͬ������DragFloat("##Y"�ĺ���, ��ImGui�Ǹ��������"##Y"����Ϊidentifier��
		// Ϊ���ò�ͬ�������ͬ���ֵ�ֵ���Ը���ͨ��UI��д, ������Ҫ�ڻ����ʼ����ID, ���ƽ�����PopId
		ImGui::PushID(label.c_str());

		// ��������߻���vector�����label
		ImGui::Columns(2);// �����˼��Labelռ���еĿռ�
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		// ���д���ο���ImGui::DragScalarN����, ��˼����Ҫ��һ�л���3��Item
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		// ��������Ĵ�С��Padding�����һ�е��и�
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// xֵ�Ĵ���, ����StyleColor�ֱ��Ӧ: ��ť������ɫ�������ͣ�ڰ�ť�ϵ���ɫ�������ťʱ����ɫ
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

		ImGui::PushFont(boldFont);
		// ��X��ť����xֵ
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);// ������Push������StyleColor���ó���

		// ��xֵ��ʾ����, ͬʱ�ṩ��ק�޸Ĺ���
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"); //(0.0f, 0.0f)��ʾû�����½�
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// yֵ�Ĵ���
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// zֵ�Ĵ���
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");// С�����2λ
		ImGui::PopItemWidth();

		// ��ǰ���PushStyleVar���Ӧ
		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			bool open = ImGuiWrapper::TreeNodeExStyle1((void*)typeid(T).hash_code(), name, treeNodeFlags);

			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::ImageButton((ImTextureID)IconManager::GetInstance().GetSettingIcon()->GetRendererID(), ImVec2{ lineHeight - 7.0f, lineHeight - 7.0f }))
			{
				// �����Popupͨ��OpenPopup��BeginPopup��EndPopupһ����Ч, �����stringΪid
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}


	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					m_SelectionContext.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<Rigidbody2DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					m_SelectionContext.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("Box Collider 2D"))
				{
					m_SelectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::MenuItem("Circle Collider 2D"))
				{
					m_SelectionContext.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<StaticMeshComponent>())
			{
				if (ImGui::MenuItem("Static Mesh Renderer"))
				{
					m_SelectionContext.AddComponent<StaticMeshComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<Rigidbody3DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 3D"))
				{
					m_SelectionContext.AddComponent<Rigidbody3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<BoxCollider3DComponent>())
			{
				if (ImGui::MenuItem("Box Collider 3D"))
				{
					m_SelectionContext.AddComponent<BoxCollider3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!m_SelectionContext.HasComponent<LightComponent>())
			{
				if (ImGui::MenuItem("Point Light"))
				{
					m_SelectionContext.AddComponent<LightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
					DrawVec3Control("Translation", component.Translation);
					glm::vec3 rotation = glm::degrees(component.Rotation);
					DrawVec3Control("Rotation", rotation);
					component.Rotation = glm::radians(rotation);
					DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
					auto& camera = component.Camera;

					ImGui::Checkbox("Primary", &component.Primary);

					const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
					const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
					// BeginCombo��ImGui����EnumPopup�ķ���
					if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
					{
						for (int i = 0; i < 2; i++)
						{
							bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
							if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
							{
								currentProjectionTypeString = projectionTypeStrings[i];
								camera.SetProjectionType((SceneCamera::ProjectionType)i);
							}

							// ������ǰ�Ѿ�ѡ���Item
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
					{
						float verticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
						if (ImGui::DragFloat("Vertical FOV", &verticalFov))
							camera.SetPerspectiveVerticalFOV(glm::radians(verticalFov));

						float orthoNear = camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near", &orthoNear))
							camera.SetPerspectiveNearClip(orthoNear);

						float orthoFar = camera.GetPerspectiveFarClip();
						if (ImGui::DragFloat("Far", &orthoFar))
							camera.SetPerspectiveFarClip(orthoFar);
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						float orthoSize = camera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &orthoSize))
							camera.SetOrthographicSize(orthoSize);

						float orthoNear = camera.GetOrthographicNearClip();
						if (ImGui::DragFloat("Near", &orthoNear))
							camera.SetOrthographicNearClip(orthoNear);

						float orthoFar = camera.GetOrthographicFarClip();
						if (ImGui::DragFloat("Far", &orthoFar))
							camera.SetOrthographicFarClip(orthoFar);

						ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
					}
			});
		
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				/*������֧��Blend�ģ��޸�һ��Quad��alpha�����޸���Zֵ�������BlendЧ���������ڴ���ɫ��͸���������ӽǡ�
				�����������ﻹû�жԻ���˳�����Ҫ�󣬶�Blend��Ҫ�Ȼ��������Զ�ģ��ٻ��ƽ��ģ�
				��������ֻ����ɫ���������ں�ɫ�Ϸ��Ż����BlendЧ����������Ҫ����Zֵ��С�ı������Ⱥ����˳��*/
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(ConfigManager::GetInstance().GetAssetsFolder()) / path;
						component.Texture = Texture2D::Create(texturePath.string());
					}
					ImGui::EndDragDropTarget();
				}



				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
			});

		DrawComponent<StaticMeshComponent>("Static Mesh Renderer", entity, [](auto& component)
			{
				ImGui::Text("Mesh Path");
				ImGui::SameLine();

				ImGui::Text(component.Path.c_str());

				ImGui::SameLine();
				if (ImGui::Button("..."))
				{
					std::string filepath = FileDialogs::OpenFile("Model (*.obj *.fbx)\0*.obj;*.fbx\0");
					if (filepath.find("Assets") != std::string::npos)
					{
						filepath = filepath.substr(filepath.find("Assets"), filepath.length());
					}
					else
					{
						// TODO: Import Model
						FLAME_CORE_ASSERT(false, "Flame Now Only support the model from Assets!");
					}
					if (!filepath.empty())
					{
						component.Mesh = Model(filepath);
						component.Path = filepath;
					}
				}

				if (ImGui::TreeNode((void*)"Material", "Material"))
				{
					const auto& materialNode = [](const char* name, Ref<Texture2D>& tex, void(*func)()) {
						if (ImGui::TreeNode((void*)name, name))
						{
							ImGui::Image((ImTextureID)tex->GetRendererID(), ImVec2(64, 64), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
							static bool use = false;
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									auto path = (const wchar_t*)payload->Data;
									std::string relativePath = (std::filesystem::path("Assets") / path).string();
									std::filesystem::path texturePath = ConfigManager::GetInstance().GetAssetsFolder() / path;
									relativePath = std::regex_replace(relativePath, std::regex("\\\\"), "/");
									tex = IconManager::GetInstance().LoadOrFindTexture(relativePath);
								}
								ImGui::EndDragDropTarget();
							}

							func();

							ImGui::TreePop();
						}
					};

					materialNode("Albedo", component.Mesh.m_AlbedoMap, []() {
						static bool use = false;
					ImGui::SameLine();
					ImGui::Checkbox("Use", &use);
						});

					materialNode("Normal", component.Mesh.m_NormalMap, []() {
						static bool use = false;
					ImGui::SameLine();
					ImGui::Checkbox("Use", &use);
						});

					materialNode("Metallic", component.Mesh.m_MetallicMap, []() {
						static bool use = false;
					ImGui::SameLine();
					ImGui::Checkbox("Use", &use);
						});

					materialNode("Roughness", component.Mesh.m_RoughnessMap, []() {
						static bool use = false;
					ImGui::SameLine();
					ImGui::Checkbox("Use", &use);
						});

					materialNode("Ambient Occlusion", component.Mesh.m_AoMap, []() {
						static bool use = false;
					ImGui::SameLine();
					ImGui::Checkbox("Use", &use);
						});

					ImGui::TreePop();
				}
			});

		DrawComponent<Rigidbody3DComponent>("Rigidbody 3D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody3DComponent::Body3DType)i;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			});

		DrawComponent<BoxCollider3DComponent>("Box Collider 3D", entity, [](auto& component)
			{

			});

		DrawComponent<LightComponent>("Point Light", entity, [](auto& component)
			{
				ImGui::Text("Light Color");
				ImGui::SameLine();
				ImGui::DragFloat3("##Light Color", (float*)&component.LightColor, 2.0f, 0.0f, 10000.0f, "%.1f");
			});


	}

}