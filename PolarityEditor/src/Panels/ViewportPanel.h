#pragma once
#include "EditorPanel.h"

#include <algorithm>

namespace Polarity
{
    enum class GizmoMode
    {
        World,
        Local
    };
    enum class GizmoOperation
    {
        Translate,
        Rotate,
        Scale,
        Bounds
    };

    class ViewportPanel : public EditorPanel
    {
    public:
        static constexpr PanelID StaticPanelID = PanelID::Viewport;

        ViewportPanel()
            : EditorPanel(StaticPanelID, 0, "Viewport") {}

        void OnDraw() override;
        void UpdateViewport(EditorCamera& editor)
        {
            if (!m_Context || !m_Context->ViewportFramebuffer)
            {
                POL_CORE_FATAL("Viewports Framebuffer is null!");
                return;
            }
            glm::vec2 viewportSize = m_Context->ViewportSize;
            if (FramebufferSpecification spec = m_Context->ViewportFramebuffer->GetSpecification();
                viewportSize.x > 0.0f && viewportSize.y > 0.0f &&
                (spec.Width != viewportSize.x || spec.Height != viewportSize.y))
            {
                m_Context->ViewportFramebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
                editor.SetViewportSize(viewportSize.x, viewportSize.y);
                m_Context->ActiveScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            }
        }
        bool IsFocused() { return m_ViewportFocused; }
        bool IsHovered() { return m_ViewportHovered; }

        void SetGizmoMode(GizmoMode mode) { m_GizmoMode = mode; }
        void SetGizmoOperation(GizmoOperation operation) { m_GizmoOperation = operation; }

    private:
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;

        GizmoMode m_GizmoMode = GizmoMode::Local;
        GizmoOperation m_GizmoOperation = GizmoOperation::Translate;
    };
}