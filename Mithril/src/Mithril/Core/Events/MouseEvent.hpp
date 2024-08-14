#pragma once

#include <sstream>

#include "Event.hpp"
#include "Mithril/Core/MouseCodes.hpp"

namespace Mithril {

    class MouseButtonEvent : public Event
    {
    public:
        MouseCode GetMouseCode() const { return m_MouseCode; }

        EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse | EventCategoryMouseButton)

    protected:
        MouseButtonEvent(MouseCode mouseCode)
            : m_MouseCode(mouseCode) {}

        MouseCode m_MouseCode;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(MouseCode mouseCode)
            : MouseButtonEvent(mouseCode) {}
        
        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonPressed " << m_MouseCode;
            return ss.str();
        }

        EVENT_CLASS(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(MouseCode mouseCode)
            : MouseButtonEvent(mouseCode) {}
        
        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseButtonReleased " << m_MouseCode;
            return ss.str();
        }

        EVENT_CLASS(MouseButtonReleased)
    };

    class MouseMovedEvent : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_X(x), m_Y(y) {}
        
        float X() const { return m_X; }
        float Y() const { return m_Y; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseMoved " << m_X << ", " << m_Y;
            return ss.str();
        }

        EVENT_CLASS(MouseMoved)
        EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

    private:
        float m_X;
        float m_Y;
    };

    class MouseScrolledEvent : public Event
    {
    public:
        MouseScrolledEvent(float x, float y)
            : m_X(x), m_Y(y) {}
        
        float X() const { return m_X; }
        float Y() const { return m_Y; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolled " << m_X << ", " << m_Y;
            return ss.str();
        }

        EVENT_CLASS(MouseScrolled)
        EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

    private:
        float m_X;
        float m_Y;
    };

}
