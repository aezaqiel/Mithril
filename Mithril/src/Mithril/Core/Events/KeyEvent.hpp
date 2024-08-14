#pragma once

#include <sstream>

#include "Event.hpp"
#include "Mithril/Core/KeyCodes.hpp"

namespace Mithril {

    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }

        EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

    protected:
        KeyEvent(KeyCode keyCode)
            : m_KeyCode(keyCode) {}

        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(KeyCode keyCode, bool repeat = false)
            : KeyEvent(keyCode), m_Repeat(repeat) {}
        
        bool Repeat() const { return m_Repeat; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressed " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS(KeyPressed)
    private:
        bool m_Repeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(KeyCode keyCode)
            : KeyEvent(keyCode) {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleased " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent (KeyCode keyCode)
            : KeyEvent(keyCode) {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTyped " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS(KeyTyped)
    };

}
