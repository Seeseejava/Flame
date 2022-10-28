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

		// ÿ��node���Դ�OpenOnArrow��flag, �����ǰentity�����Ǳ�ѡ���go, ��ô�����һ��selected flag
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

		// �����TreeNodeEx����ImGui���������HashCode(GUID), ����һ��TreeNode, ����������Ҫһ��
		// void*ָ��, ����ֱ�Ӱ�entity��idת��void*��������
		// exӦ����expanded����˼, �����ж�go��Ӧ��Node�Ƿ���չ��״̬
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str()); //������Ϊָ����64λ�ģ�Ϊ�˳������⣬��ת����64λ

		// ������������item��, �ҵ����������, �򷵻�true
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		// ����˽ڵ���expanded״̬, ��ô��Ҫ����loop������ȥ
		// ����Ŀǰû����ʽentity, ���������չ���Ķ����ٻ���һ����ͬ���ӽڵ�
		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			// TreePopò���Ǹ������Ĳ���, ����ÿ���ڵ���ƽ���ʱҪ���ô˺���
			ImGui::TreePop();
		}

	}

}