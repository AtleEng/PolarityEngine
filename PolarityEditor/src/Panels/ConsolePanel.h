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
        const char* GetName() const override { return "Console"; }

        void OnStart() override;
        void OnDraw() override;
		void Clear();
		void AddLog(const char* fmt, ...) IM_FMTARGS(2);
		void Bind_Log(const LogEvent& e);
    private:
		EditorLog m_EditorLog;
    };
}