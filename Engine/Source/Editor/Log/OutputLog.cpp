#include "Editor/Log/OutputLog.h"
#include "Runtime/Core/Log.h"

#include <spdlog/spdlog.h>
#include <imgui/imgui_internal.h>

namespace Flame
{
	OutputLog::OutputLog()
	{
		ClearLog();
		memset(InputBuf, 0, sizeof(InputBuf));
		HistoryPos = -1;

		// "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
		Commands.push_back("HELP");
		Commands.push_back("HISTORY");
		Commands.push_back("CLEAR");
		Commands.push_back("CLASSIFY");
		ScrollToBottom = false;
	}

	OutputLog::~OutputLog()
	{
		ClearLog();
		for (int i = 0; i < History.Size; i++)
			free(History[i]);
	}

	void OutputLog::ClearLog()
	{
		for (int i = 0; i < Items.Size; i++)
			free(Items[i]);
		Items.clear();
	}

	void OutputLog::AddLog(const char* fmt, ...)
	{
		// FIXME-OPT
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		buf[IM_ARRAYSIZE(buf) - 1] = 0;
		va_end(args);
		Items.push_back(Strdup(buf));
	}

	void OutputLog::OnImGuiRender(bool* pOpen)
	{
		// Filters
		static bool bShowInfo = true;
		static bool bShowTrace = true;
		static bool bShowWarning = true;
		static bool bShowError = true;

		// Settings
		static bool bAutoScroll = true;

		if (!ImGui::Begin("Output Log", pOpen))
		{
			ImGui::End();
			return;
		}

		bool copy_to_clipboard = false;

		// Options menu
		if (ImGui::BeginPopup("Filters"))
		{
			ImGui::Checkbox("Info", &bShowInfo);
			ImGui::Checkbox("Trace", &bShowTrace);
			ImGui::Checkbox("Warning", &bShowWarning);
			ImGui::Checkbox("Error", &bShowError);
			ImGui::EndPopup();
		}

		// Options, Filter
		if (ImGui::Button("Filters"))
			ImGui::OpenPopup("Filters");
		ImGui::SameLine();
		
		ImGui::SetNextItemWidth(380.0);
		bool value_changed = ImGui::InputTextWithHint("##SearchLog", "Search Log", Filter.InputBuf, IM_ARRAYSIZE(Filter.InputBuf));
		if (value_changed)
			Filter.Build();

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const char* settingsLabel = "Settings";

		if (ImGui::BeginPopup("Settings"))
		{
			ImGui::Checkbox("Auto-scroll", &bAutoScroll);
			ImGui::EndPopup();
		}

		// TODO: add setting Icon
		ImGui::SameLine(contentRegionAvailable.x - std::strlen(settingsLabel) * GImGui->Font->FontSize * 0.5f + 20.0f);
		if (ImGui::Button("Settings"))
			ImGui::OpenPopup("Settings");

		ImGui::Separator();

		// Reserve enough left-over height for 1 separator + 1 input text
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear Log")) ClearLog();
			if (ImGui::Selectable("Copy")) copy_to_clipboard = true;
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		if (copy_to_clipboard)
			ImGui::LogToClipboard();
		for (int i = 0; i < Items.Size; i++)
		{
			const char* item = Items[i];

			// item is one log
			if (!Filter.PassFilter(item))
				continue;

			// Normally you would store more information in your item than just a string.
			// (e.g. make Items[] an array of structure, store color/type etc.)
			ImVec4 color;
			bool has_color = false;
			if (strstr(item, "[info]"))
			{
				if (!bShowInfo)
				{
					// don't show info log, others are similarly
					continue;
				}
				color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				has_color = true;
			}
			else if (strstr(item, "[trace]"))
			{
				if (!bShowTrace) continue;
				color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
				has_color = true;
			}
			else if (strstr(item, "[warning]"))
			{
				if (!bShowWarning) continue;
				color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
				has_color = true;
			}
			else if (strstr(item, "[error]"))
			{
				if (!bShowError) continue;
				color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
				has_color = true;
			}
			if (has_color)
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item);
			if (has_color)
				ImGui::PopStyleColor();
		}
		if (copy_to_clipboard)
			ImGui::LogFinish();

		if (ScrollToBottom || (bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		ScrollToBottom = false;

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		// Command-line
		bool reclaim_focus = false;
		ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		auto TextEditCallbackStub = [](ImGuiInputTextCallbackData* data) {
			OutputLog* console = (OutputLog*)data->UserData;
			return console->TextEditCallback(data);
		};

		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
		if (ImGui::InputTextWithHint("##ConsoleInput", "Enter Console Command", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, TextEditCallbackStub, (void*)this))
		{
			char* s = InputBuf;
			Strtrim(s);
			if (s[0])
				ExecCommand(s);
			strcpy(s, "");
			reclaim_focus = true;
		}

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if (reclaim_focus)
			ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

		ImGui::End();
	}

	void OutputLog::ExecCommand(const char* command_line)
	{
		AddLog("# %s\n", command_line);

		// Insert into history. First find match and delete it so it can be pushed to the back.
		// This isn't trying to be smart or optimal.
		HistoryPos = -1;
		for (int i = History.Size - 1; i >= 0; i--)
			if (Stricmp(History[i], command_line) == 0)
			{
				free(History[i]);
				History.erase(History.begin() + i);
				break;
			}
		History.push_back(Strdup(command_line));

		// Process command
		if (Stricmp(command_line, "CLEAR") == 0)
		{
			ClearLog();
		}
		else if (Stricmp(command_line, "HELP") == 0)
		{
			AddLog("Commands:");
			for (int i = 0; i < Commands.Size; i++)
				AddLog("- %s", Commands[i]);
		}
		else if (Stricmp(command_line, "HISTORY") == 0)
		{
			int first = History.Size - 10;
			for (int i = first > 0 ? first : 0; i < History.Size; i++)
				AddLog("%3d: %s\n", i, History[i]);
		}
		else if (Stricmp(command_line, "FLAME") == 0)
		{
			FLAME_CORE_INFO("GOOD!");
		}
		else
		{
			AddLog("Unknown command: '%s'\n", command_line);
		}

		// On command input, we scroll to bottom even if AutoScroll==false
		ScrollToBottom = true;
	}

	int OutputLog::TextEditCallback(ImGuiInputTextCallbackData* data)
	{
		//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
		switch (data->EventFlag)
		{
		case ImGuiInputTextFlags_CallbackCompletion:
		{
			// Example of TEXT COMPLETION

			// Locate beginning of current word
			const char* word_end = data->Buf + data->CursorPos;
			const char* word_start = word_end;
			while (word_start > data->Buf)
			{
				const char c = word_start[-1];
				if (c == ' ' || c == '\t' || c == ',' || c == ';')
					break;
				word_start--;
			}

			// Build a list of candidates
			ImVector<const char*> candidates;
			for (int i = 0; i < Commands.Size; i++)
				if (Strnicmp(Commands[i], word_start, (int)(word_end - word_start)) == 0)
					candidates.push_back(Commands[i]);

			if (candidates.Size == 0)
			{
				// No match
				AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
			}
			else if (candidates.Size == 1)
			{
				// Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0]);
				data->InsertChars(data->CursorPos, " ");
			}
			else
			{
				// Multiple matches. Complete as much as we can..
				// So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
				int match_len = (int)(word_end - word_start);
				for (;;)
				{
					int c = 0;
					bool all_candidates_matches = true;
					for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
						if (i == 0)
							c = toupper(candidates[i][match_len]);
						else if (c == 0 || c != toupper(candidates[i][match_len]))
							all_candidates_matches = false;
					if (!all_candidates_matches)
						break;
					match_len++;
				}

				if (match_len > 0)
				{
					data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
					data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
				}

				// List matches
				AddLog("Possible matches:\n");
				for (int i = 0; i < candidates.Size; i++)
					AddLog("- %s\n", candidates[i]);
			}

			break;
		}
		case ImGuiInputTextFlags_CallbackHistory:
		{
			// Example of HISTORY
			const int prev_history_pos = HistoryPos;
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (HistoryPos == -1)
					HistoryPos = History.Size - 1;
				else if (HistoryPos > 0)
					HistoryPos--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (HistoryPos != -1)
					if (++HistoryPos >= History.Size)
						HistoryPos = -1;
			}

			// A better implementation would preserve the data on the current input line along with cursor position.
			if (prev_history_pos != HistoryPos)
			{
				const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
				data->DeleteChars(0, data->BufTextLen);
				data->InsertChars(0, history_str);
			}
		}
		}
		return 0;
	}
}