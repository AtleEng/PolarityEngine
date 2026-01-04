#include "polpch.h"
#include "ConsolePanel.h"



namespace Polarity
{
	void ConsolePanel::OnDraw()
	{
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);
		

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &m_EditorLog.AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		m_EditorLog.Filter.Draw("Filter", -100.0f);

		ImGui::Separator();

		if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (clear)
				ClearLog();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = m_EditorLog.Buf.begin();
			const char* buf_end = m_EditorLog.Buf.end();
			if (m_EditorLog.Filter.IsActive())
			{
				for (int line_no = 0; line_no < m_EditorLog.LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + m_EditorLog.LineOffsets[line_no];
					const char* line_end = (line_no + 1 < m_EditorLog.LineOffsets.Size) ? (buf + m_EditorLog.LineOffsets[line_no + 1] - 1) : buf_end;
					if (m_EditorLog.Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				ImGuiListClipper clipper;
				clipper.Begin(m_EditorLog.LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + m_EditorLog.LineOffsets[line_no];
						const char* line_end = (line_no + 1 < m_EditorLog.LineOffsets.Size) ? (buf + m_EditorLog.LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			if (m_EditorLog.AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		ImGui::EndChild();
		ImGui::End();
	}

	void ConsolePanel::ClearLog()
	{
		m_EditorLog.Buf.clear();
		m_EditorLog.LineOffsets.clear();
		m_EditorLog.LineOffsets.push_back(0);
	}

	void ConsolePanel::BindLog(const LogEvent& e)
	{
		AddLog("%s %s %s\n",
			e.prefix.c_str(),
			e.time.c_str(),
			e.message.c_str());
	}

	void ConsolePanel::AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = m_EditorLog.Buf.size();
		va_list args;
		va_start(args, fmt);
		m_EditorLog.Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = m_EditorLog.Buf.size(); old_size < new_size; old_size++)
			if (m_EditorLog.Buf[old_size] == '\n')
				m_EditorLog.LineOffsets.push_back(old_size + 1);
	}
}