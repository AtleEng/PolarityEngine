#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class ViewportPanel : public EditorPanel
    {
    public:
        static constexpr PanelID StaticPanelID = PanelID::Viewport;
        static constexpr bool AllowMultiple() { return true; }

        ViewportPanel(uint32_t instanceID)
            : EditorPanel(StaticPanelID, instanceID, "Viewport") {}

        void OnDraw() override;
        void UpdateViewport()
        {
            if (!m_Context || !m_Context->ViewportFramebuffer)
            {
                LOG_MAJOR_ERROR("Viewports Framebuffer is null!");
                return;
            }
            //------------ Resize window --------------------------------
            glm::vec2 viewportSize = m_Context->ViewportSize;
            if (FramebufferSpecification spec = m_Context->ViewportFramebuffer->GetSpecification();
                viewportSize.x > 0.0f && viewportSize.y > 0.0f &&
                (spec.Width != viewportSize.x || spec.Height != viewportSize.y))
            {
                m_Context->ViewportFramebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
                m_Context->ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

                //m_cameraController.OnResize(m_viewportSize.x, m_viewportSize.y);
            }

            //if (m_ViewportFocused)
                //m_cameraController.OnUpdate(tS);
        }
    private:
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
    };
}