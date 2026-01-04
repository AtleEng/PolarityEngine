#pragma once
#include "EditorPanel.h"
#include "imgui/imgui.h"

namespace Polarity
{
	struct EditorLog
	{
		ImGuiTextBuffer Buf;
		ImGuiTextFilter Filter;
		ImVector<int>   LineOffsets;		// Index to lines offset. We maintain this with AddLog() calls.
		bool            AutoScroll = true;  // Keep scrolling if already at the bottom.
	};

    class ConsolePanel : public EditorPanel
    {
    public:
		static constexpr PanelID StaticPanelID = PanelID::Console;

		ConsolePanel()
			: EditorPanel(StaticPanelID, 0, "Console") {}

        void OnDraw() override;

		void ClearLog();
		void AddLog(const char* fmt, ...) IM_FMTARGS(2);
		void BindLog(const LogEvent& e);
    private:
		EditorLog m_EditorLog;
    };
}