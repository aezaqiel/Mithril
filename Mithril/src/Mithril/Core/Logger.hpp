#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Mithril {

    class Logger
    {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

}

#ifndef NDEBUG
    #define M_CORE_TRACE(...)    ::Mithril::Logger::GetCoreLogger()->trace(__VA_ARGS__)
    #define M_CORE_DEBUG(...)    ::Mithril::Logger::GetCoreLogger()->debug(__VA_ARGS__)
    #define M_CORE_INFO(...)     ::Mithril::Logger::GetCoreLogger()->info(__VA_ARGS__)
    #define M_CORE_WARN(...)     ::Mithril::Logger::GetCoreLogger()->warn(__VA_ARGS__)
    #define M_CORE_ERROR(...)    ::Mithril::Logger::GetCoreLogger()->error(__VA_ARGS__)
    #define M_CORE_CRITICAL(...) ::Mithril::Logger::GetCoreLogger()->critical(__VA_ARGS__)

    #define MTRACE(...)    ::Mithril::Logger::GetClientLogger()->trace(__VA_ARGS__)
    #define MDEBUG(...)    ::Mithril::Logger::GetClientLogger()->debug(__VA_ARGS__)
    #define MINFO(...)     ::Mithril::Logger::GetClientLogger()->info(__VA_ARGS__)
    #define MWARN(...)     ::Mithril::Logger::GetClientLogger()->warn(__VA_ARGS__)
    #define MERROR(...)    ::Mithril::Logger::GetClientLogger()->error(__VA_ARGS__)
    #define MCRITICAL(...) ::Mithril::Logger::GetClientLogger()->critical(__VA_ARGS__)
#else
    #define M_CORE_TRACE(...)
    #define M_CORE_DEBUG(...)
    #define M_CORE_INFO(...)
    #define M_CORE_WARN(...)
    #define M_CORE_ERROR(...)
    #define M_CORE_CRITICAL(...)

    #define MTRACE(...)
    #define MDEBUG(...)
    #define MINFO(...)
    #define MWARN(...)
    #define MERROR(...)
    #define MCRITICAL(...)
#endif
