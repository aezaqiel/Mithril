#pragma once

#include <sstream>

#include "Event.hpp"

namespace Mithril {

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height) {}
        
        uint32_t Width() const { return m_Width; }
        uint32_t Height() const { return m_Height; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResize " << m_Width << ", " << m_Height;
            return ss.str();
        }
        
        EVENT_CLASS(WindowResize)
        EVENT_CATEGORY(EventCategoryWindow)

    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };

    class WindowMoveEvent : public Event
    {
    public:
        WindowMoveEvent(uint32_t x, uint32_t y)
            : m_X(x), m_Y(y) {}
        
        uint32_t X() const { return m_X; }
        uint32_t Y() const { return m_Y; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMove " << m_X << ", " << m_Y;
            return ss.str();
        }

        EVENT_CLASS(WindowMove)
        EVENT_CATEGORY(EventCategoryWindow)

    private:
        uint32_t m_X;
        uint32_t m_Y;
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
