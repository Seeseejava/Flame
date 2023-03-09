#include "ImGuiWrapper.h"

#include <string>

namespace Flame::ImGuiWrapper
{
    bool InputTextLeft(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        ImGui::Text(label);
        ImGui::SameLine();
        std::string InputLabel = std::string("##") + std::string(label);
        return ImGui::InputText(InputLabel.c_str(), buf, buf_size, flags, callback, user_data);
    }

    bool TreeNodeExStyle1(const void* str_id, const std::string& name, ImGuiTreeNodeFlags flags)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f, 4.0f });
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void*)str_id, flags, name.c_str());
        ImGui::PopStyleVar();
        return open;
    }

	// SubNode
	bool TreeNodeExStyle2(const void* str_id, const std::string& name, ImGuiTreeNodeFlags flags)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 2.0f, 2.0f });
		bool open = ImGui::TreeNodeEx((void*)str_id, flags, name.c_str());
		ImGui::PopStyleVar();
		return open;
	}

	void CenteredSeparator(float width)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;
		ImGuiContext& g = *GImGui;

		// Horizontal Separator
		float x1, x2;
		if (window->DC.CurrentColumns == NULL && (width == 0))
		{
			// Span whole window
			///x1 = window->Pos.x; // This fails with SameLine(); CenteredSeparator();
			// Nah, we have to detect if we have a sameline in a different way
			x1 = window->DC.CursorPos.x;
			x2 = x1 + window->Size.x;
		}
		else
		{
			// Start at the cursor
			x1 = window->DC.CursorPos.x;
			if (width != 0) {
				x2 = x1 + width;
			}
			else
			{
				x2 = window->ClipRect.Max.x;
				// Pad right side of columns (except the last one)
				if (window->DC.CurrentColumns && (window->DC.CurrentColumns->Current < window->DC.CurrentColumns->Count - 1))
					x2 -= g.Style.ItemSpacing.x;
			}
		}
		float y1 = window->DC.CursorPos.y + int(window->DC.CurrLineSize.y / 2.0f);
		float y2 = y1 + 1.0f;

		window->DC.CursorPos.x += width; //+ g.Style.ItemSpacing.x;

		if (!window->DC.ItemWidthStack.empty())
			x1 += window->DC.Indent.x;

		const ImRect bb(ImVec2(x1, y1), ImVec2(x2, y2));
		ImGui::ItemSize(ImVec2(0.0f, 0.0f)); // NB: we don't provide our width so that it doesn't get feed back into AutoFit, we don't provide height to not alter layout.
		if (!ImGui::ItemAdd(bb, NULL))
		{
			return;
		}

		window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), ImGui::GetColorU32(ImGuiCol_Border));
	}
}