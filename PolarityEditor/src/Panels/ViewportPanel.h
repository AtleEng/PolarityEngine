#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class ViewportPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "Viewport"; }

        void OnStart() override {}
        void OnDraw() override;
        void UpdateViewport()
        {
            if (!m_Context || !m_Context->ViewportFramebuffer)
            {
                LOG_MAJOR_ERROR("Viewports Framebuffer is null!");
                return;
            }
            //------------ Resize window --------------------------------
            if (FramebufferSpecification spec = m_Context->ViewportFramebuffer->GetSpecification();
                m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
                (spec.Width != m_viewportSize.x || spec.Height != m_viewportSize.y))
            {
                m_Context->ViewportFramebuffer->Resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
                m_Context->ActiveScene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);

                //m_cameraController.OnResize(m_viewportSize.x, m_viewportSize.y);
            }

            //if (m_ViewportFocused)
                //m_cameraController.OnUpdate(tS);
        }
    private:
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
        glm::vec2 m_viewportSize = { 1280, 720 };
    };
}