#include "Logger.hpp"

#include <vector>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Mithril {

    Ref<spdlog::logger> Logger::s_CoreLogger;
    Ref<spdlog::logger> Logger::s_ClientLogger;

    void Logger::Init()
    {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("Mithril.log", true));

        logSinks[0]->set_pattern("[%H:%M:%S %z] [%n] [%^%l%$] [thread %t] %v");
        logSinks[1]->set_pattern("[%H:%M:%S %z] [%n] [%^%l%$] [thread %t] %v");

        s_CoreLogger = CreateRef<spdlog::logger>("MITHRIL", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);

        s_ClientLogger = CreateRef<spdlog::logger>("CLIENT", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ClientLogger);
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
    }

}