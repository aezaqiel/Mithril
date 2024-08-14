#pragma once

#include <string>
#include <concepts>
#include <ostream>

#define BIT(x) (1 << x)

namespace Mithril {

    enum class EventType
    {
        None = 0,
        AppTick, AppUpdate, AppRender,
        WindowResize, WindowMove, WindowClose, WindowMinimize, WindowRestore, WindowFocus, WindowLostFocus,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryWindow      = BIT(1),
        EventCategoryInput       = BIT(2),
        EventCategoryKeyboard    = BIT(3),
        EventCategoryMouse       = BIT(4),
        EventCategoryMouseButton = BIT(5)
    };

    #define EVENT_CLASS(type) static EventType GetStaticType() { return EventType::type; } \
                              virtual EventType GetEventType() const override { return GetStaticType(); } \
                              virtual const char* GetName() const override { return #type; }
                            
    #define EVENT_CATEGORY(category) virtual int32_t GetCategoryFlags() const override { return category; }

    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int32_t GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool InCategory(EventCategory category) const
        {
            return GetCategoryFlags() & category;
        }
    };

    template<typename T>
    concept EventTypeConcept = std::derived_from<T, Event> && requires(T event) { { T::GetStaticType() } -> std::same_as<EventType>; };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event)
            : m_Event(event) {}
        
        template<EventTypeConcept T, std::invocable<T&> F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType()) {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }

            return false;
        }

    private:
        Event& m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }

}

