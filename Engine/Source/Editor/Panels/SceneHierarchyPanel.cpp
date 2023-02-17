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
				// 1代表鼠标右键(0代表左键、2代表中键), bool over_item为false, 意味着这个窗口只在空白处点击才会触发 
				// 后续应该允许在item上点击, 无非此时创建的是子GameObject
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


		// 每个node都自带OpenOnArrow的flag, 如果当前entity正好是被选择的go, 那么还会多一个selected flag
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // 覆盖整个空间
		// 这里的TreeNodeEx会让ImGui基于输入的HashCode(GUID), 绘制一个TreeNode, 由于这里需要一个
		// void*指针, 这里直接把entity的id转成void*给它即可
		// ex应该是expanded的意思, 用于判断go对应的Node是否处于展开状态
		std::string label = std::string("##") + std::string(tag);
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, ""); //这里因为指针是64位的，为了出现问题，先转换成64位

		// 如果鼠标悬浮在item上, 且点击了鼠标左键, 则返回true
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

		// 如果此节点是expanded状态, 那么需要继续loop到里面去
		// 由于目前没有链式entity, 所以这里把展开的对象再绘制一个相同的子节点
		if (opened)
		{
			// TreePop貌似是个结束的操作, 好像每个节点绘制结束时要调用此函数
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

		// Translation、Scale都会有相同的类似DragFloat("##Y"的函数, 而ImGui是根据输入的"##Y"来作为identifier的
		// 为了让不同组件的相同名字的值可以各自通过UI读写, 这里需要在绘制最开始加入ID, 绘制结束后PopId
		ImGui::PushID(label.c_str());

		// 先在最左边绘制vector代表的label
		ImGui::Columns(2);// 大概意思是Label占两列的空间
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		// 这行代码参考自ImGui::DragScalarN函数, 意思是我要在一行绘制3个Item
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		// 基于字体的大小和Padding算出这一行的行高
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		// x值的处理, 三个StyleColor分别对应: 按钮本身颜色、鼠标悬停在按钮上的颜色、点击按钮时的颜色
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

		ImGui::PushFont(boldFont);
		// 按X按钮重置x值
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);// 把上面Push的三个StyleColor给拿出来

		// 把x值显示出来, 同时提供拖拽修改功能
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f"); //(0.0f, 0.0f)表示没有上下界
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// y值的处理
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

		// z值的处理
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");// 小数点后2位
		ImGui::PopItemWidth();

		// 与前面的PushStyleVar相对应
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
				// 这里的Popup通过OpenPopup、BeginPopup和EndPopup一起生效, 输入的string为id
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
					// BeginCombo是ImGui绘制EnumPopup的方法
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

							// 高亮当前已经选择的Item
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
				/*这里是支持Blend的，修改一个Quad的alpha，再修改其Z值，会产生Blend效果，类似于带颜色的透镜来看的视角。
				但是由于这里还没有对绘制顺序进行要求，而Blend需要先绘制离相机远的，再绘制近的，
				所以这里只有绿色的正方形在红色上方才会出现Blend效果，后续需要根据Z值大小改变物体先后绘制顺序。*/
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