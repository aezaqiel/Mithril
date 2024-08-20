#pragma once

#include <sstream>

#include "Event.hpp"

namespace Mithril {

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(u32 width, u32 height)
            : m_Width(width), m_Height(height) {}
        
        u32 Width() const { return m_Width; }
        u32 Height() const { return m_Height; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResize " << m_Width << ", " << m_Height;
            return ss.str();
        }
        
        EVENT_CLASS(WindowResize)
        EVENT_CATEGORY(EventCategoryWindow)

    private:
        u32 m_Width;
        u32 m_Height;
    };

    class WindowMoveEvent : public Event
    {
    public:
        WindowMoveEvent(u32 x, u32 y)
            : m_X(x), m_Y(y) {}
        
        u32 X() const { return m_X; }
        u32 Y() const { return m_Y; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMove " << m_X << ", " << m_Y;
            return ss.str();
        }

        EVENT_CLASS(WindowMove)
        EVENT_CATEGORY(EventCategoryWindow)

    private:
        u32 m_X;
        u32 m_Y;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS(WindowClose)
        EVENT_CATEGORY(EventCategoryWindow)
    };

    class WindowMinimizeEvent : public Event
    {
    public:
        WindowMinimizeEvent() = default;

        EVENT_CLASS(WindowMinimize)
        EVENT_CATEGORY(EventCategoryWindow)
    };

    class WindowRestoreEvent : public Event
    {
    public:
        WindowRestoreEvent() = default;

        EVENT_CLASS(WindowRestore)
        EVENT_CATEGORY(EventCategoryWindow)
    };

    class WindowFocusEvent: public Event
    {
    public:
        WindowFocusEvent() = default;

        EVENT_CLASS(WindowFocus)
        EVENT_CATEGORY(EventCategoryWindow)
    };

    class WindowLostFocusEvent: public Event
    {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS(WindowLostFocus)
        EVENT_CATEGORY(EventCategoryWindow)
    };

}
