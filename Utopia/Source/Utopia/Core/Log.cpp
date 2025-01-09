#include "Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>

#if !defined(UT_HAS_CONSOLE)
#define UT_HAS_CONSOLE (!UT_DIST)
#endif

namespace Utopia {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);

        static const std::string logsDirectory = "logs";
        if (!std::filesystem::exists(logsDirectory))
            std::filesystem::create_directories(logsDirectory);

        std::vector<spdlog::sink_ptr> coreSinks{
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/CORE.log", true)
        };

#if UT_HAS_CONSOLE
        coreSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

        coreSinks[0]->set_pattern("[%T] [%l] %n: %v");
#if UT_HAS_CONSOLE
        coreSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

        s_CoreLogger = std::make_shared<spdlog::logger>("CORE", coreSinks.begin(), coreSinks.end());
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);

        std::vector<spdlog::sink_ptr> clientSinks{
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/APP.log", true)
        };

#if UT_HAS_CONSOLE
        clientSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

        clientSinks[0]->set_pattern("[%T] [%l] %n: %v");
#if UT_HAS_CONSOLE
        clientSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

        s_ClientLogger = std::make_shared<spdlog::logger>("CLIENT", clientSinks.begin(), clientSinks.end());
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
    }

    void Log::Shutdown() noexcept
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        s_ClientLogger.reset();
        s_CoreLogger.reset();
        spdlog::drop_all();
    }

    void Log::PrintMessageTag(Type type, Level level, std::string_view tag, std::string_view message)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);

        auto it = s_EnabledTags.find(std::string(tag));
        if (it != s_EnabledTags.end())
        {
            const auto& detail = it->second;
            if (detail.Enabled && detail.LevelFilter <= level)
            {
                auto logger = (type == Type::Core) ? s_CoreLogger : s_ClientLogger;
                if (!logger)
                    return;

                switch (level)
                {
                case Level::Trace:
                    logger->trace("[{}] {}", tag, message);
                    break;
                case Level::Info:
                    logger->info("[{}] {}", tag, message);
                    break;
                case Level::Warn:
                    logger->warn("[{}] {}", tag, message);
                    break;
                case Level::Error:
                    logger->error("[{}] {}", tag, message);
                    break;
                case Level::Fatal:
                    logger->critical("[{}] {}", tag, message);
                    break;
                }
            }
        }
    }

    void Log::PrintAssertMessage(Type type, std::string_view prefix)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);

        auto logger = (type == Type::Core) ? s_CoreLogger : s_ClientLogger;
        if (!logger)
            return;

        logger->error("{}", prefix);

#if defined(UT_ASSERT_MESSAGE_BOX) && UT_ASSERT_MESSAGE_BOX && defined(UT_PLATFORM_WINDOWS)
        MessageBoxA(nullptr, "Assertion failed!", "Utopia Assert", MB_OK | MB_ICONERROR);
#endif
    }

    const char* Log::LevelToString(Level level) noexcept
    {
        switch (level)
        {
        case Level::Trace: return "Trace";
        case Level::Info:  return "Info";
        case Level::Warn:  return "Warn";
        case Level::Error: return "Error";
        case Level::Fatal: return "Fatal";
        default:           return "Unknown";
        }
    }

    Log::Level Log::LevelFromString(std::string_view string) noexcept
    {
        if (string == "Trace") return Level::Trace;
        if (string == "Info")  return Level::Info;
        if (string == "Warn")  return Level::Warn;
        if (string == "Error") return Level::Error;
        if (string == "Fatal") return Level::Fatal;

        return Level::Trace; // Default level
    }

} // namespace Utopia
