#include "polpch.h"
#include "PanelManager.h"

namespace Polarity {

    void PanelManager::SetContext(EditorContext& ctx)
    {
        for (auto& [id, panels] : m_Panels)
        {
            for (auto it = panels.begin(); it != panels.end(); )
            {
                (*it)->SetContext(ctx);
            }
        }
    }

	void PanelManager::OnDraw()
	{
        for (auto& [id, panels] : m_Panels)
        {
            for (auto it = panels.begin(); it != panels.end(); )
            {
                if (!(*it)->IsOpen())
                {
                    (*it)->OnDetach();
                    it = panels.erase(it);
                }
                else
                {
                    (*it)->OnDraw();
                    ++it;
                }
            }
        }
	}
	void PanelManager::Clear()
	{
        for (auto& [id, panels] : m_Panels)
        {
            for (auto it = panels.begin(); it != panels.end(); )
            {
                (*it)->Close();
                ++it;
            }
        }
	}
}