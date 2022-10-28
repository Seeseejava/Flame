#include "SceneHierarchyPanel.h"

#include <imgui/imgui.h>


namespace Flame {

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);
			});

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		// 每个node都自带OpenOnArrow的flag, 如果当前entity正好是被选择的go, 那么还会多一个selected flag
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		// 这里的TreeNodeEx会让ImGui基于输入的HashCode(GUID), 绘制一个TreeNode, 由于这里需要一个
		// void*指针, 这里直接把entity的id转成void*给它即可
		// ex应该是expanded的意思, 用于判断go对应的Node是否处于展开状态
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str()); //这里因为指针是64位的，为了出现问题，先转换成64位

		// 如果鼠标悬浮在item上, 且点击了鼠标左键, 则返回true
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		// 如果此节点是expanded状态, 那么需要继续loop到里面去
		// 由于目前没有链式entity, 所以这里把展开的对象再绘制一个相同的子节点
		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			// TreePop貌似是个结束的操作, 好像每个节点绘制结束时要调用此函数
			ImGui::TreePop();
		}

	}

}