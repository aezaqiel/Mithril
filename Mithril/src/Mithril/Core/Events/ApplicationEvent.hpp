#pragma once

#include "Event.hpp"

namespace Mithril {

    class AppTickEvent : public Event
    {
    public:
        AppTickEvent() = default;

        EVENT_CLASS(AppTick)
        EVENT_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public Event
    {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS(AppUpdate)
        EVENT_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent: public Event
    {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS(AppRender)
        EVENT_CATEGORY(EventCategoryApplication)
    };

}
