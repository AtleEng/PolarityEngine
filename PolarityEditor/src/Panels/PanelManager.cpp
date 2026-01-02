#include "polpch.h"
#include "PanelManager.h"

namespace Polarity {
	std::vector<Ref<EditorPanel>> PanelManager::m_Panels;

	void PanelManager::Init()
	{
		
	}
	void PanelManager::OpenPanel()
	{
	}
	void PanelManager::ClosePanel()
	{
	}
	void PanelManager::OnDraw()
	{
		for (int i = 0; i < m_Panels.size(); i++)
		{
			m_Panels[i]->OnDraw();
		}
	}
	void PanelManager::SetContext(EditorContext& ctx)
	{
		for (int i = 0; i < m_Panels.size(); i++)
		{
			m_Panels[i]->SetContext(ctx);
		}
	}
}