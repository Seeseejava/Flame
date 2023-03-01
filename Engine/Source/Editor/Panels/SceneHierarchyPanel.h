#pragma once

#include "Flame.h"

namespace Flame {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender(bool* pOpen, bool* pOpenProperties);

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template <typename componentType>
		void MenuAddComponent(const char* menuName, const char* menuItemName);
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;// ┤˙▒ÝSelected Entity
	};

}