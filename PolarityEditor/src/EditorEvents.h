#pragma once
#include "engine/events/Event.h"

#include <filesystem>

namespace Polarity
{

    enum class EditorEventType
    {
        OpenScene,
        SaveScene,
        NewScene,
        EntitySelected
    };

    class EditorEvent : public Event
    {
    public:
        EditorEvent(EditorEventType type)
            : m_Type(type) {
        }

        EditorEventType GetEditorType() const { return m_Type; }

        EVENT_CLASS_TYPE(AppUpdate) // reuse existing type
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        EditorEventType m_Type;
    };

    class OpenSceneEditorEvent : public EditorEvent
    {
    public:
        OpenSceneEditorEvent(const std::filesystem::path& path)
            : EditorEvent(EditorEventType::OpenScene), m_Path(path) {
        }

        const std::filesystem::path& GetPath() const { return m_Path; }

    private:
        std::filesystem::path m_Path;
    };
}