#pragma once

// Utopia's logging system is based on the spdlog logging library (https://github.com/gabime/spdlog)

#include "LogCustomFormatters.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <mutex>

#define UT_ASSERT_MESSAGE_BOX (!UT_DIST && UT_PLATFORM_WINDOWS)

#if UT_ASSERT_MESSAGE_BOX
#ifdef UT_PLATFORM_WINDOWS
#include <Windows.h>
#endif
#endif

namespace Utopia {

    class Log
    {
    public:
        // Log Type and Level Enums
        enum class Type : uint8_t
        {
            Core = 0,
            Client
        };

        enum class Level : uint8_t
        {
            Trace = 0,
            Info,
            Warn,
            Error,
            Fatal
        };

        // Struct to hold tag details
        struct TagDetails
        {
            bool Enabled = true;
            Level LevelFilter = Level::Trace;
        };

    public:
        // Initialize and Shutdown logging system
        static void Init();
        static void Shutdown() noexcept;

        // Accessors for the loggers
        [[nodiscard]] static std::shared_ptr<spdlog::logger>& GetCoreLogger();
        [[nodiscard]] static std::shared_ptr<spdlog::logger>& GetClientLogger();

        // Tag management
        [[nodiscard]] static bool HasTag(const std::string& tag);
        [[nodiscard]] static std::map<std::string, TagDetails>& EnabledTags();

        // Logging functions
        template<typename... Args>
        static void PrintMessageTag(Type type, Level level, std::string_view tag, std::format_string<Args...> format, Args&&... args);

        static void PrintMessageTag(Type type, Level level, std::string_view tag, std::string_view message);

        // Assertion-based logging
        template<typename... Args>
        static void PrintAssertMessage(Type type, std::string_view prefix, Args&&... args);

        static void PrintAssertMessage(Type type, std::string_view prefix);

    private:
        // Utility functions
        [[nodiscard]] static const char* LevelToString(Level level) noexcept;
        [[nodiscard]] static Level LevelFromString(std::string_view string) noexcept;

    private:
        // Logger instances
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;

        // Enabled tags mapped by tag name
        inline static std::map<std::string, TagDetails> s_EnabledTags;

        // Mutex for thread-safe operations
        inline static std::mutex s_Mutex;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Inline Template Definitions                                                                                      //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Implementation of variadic PrintMessageTag
    template<typename... Args>
    void Log::PrintMessageTag(Type type, Level level, std::string_view tag, std::format_string<Args...> format, Args&&... args)
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

                std::string formattedMsg = std::format(format, std::forward<Args>(args)...);

                switch (level)
                {
                case Level::Trace:
                    logger->trace("[{}] {}", tag, formattedMsg);
                    break;
                case Level::Info:
                    logger->info("[{}] {}", tag, formattedMsg);
                    break;
                case Level::Warn:
                    logger->warn("[{}] {}", tag, formattedMsg);
                    break;
                case Level::Error:
                    logger->error("[{}] {}", tag, formattedMsg);
                    break;
                case Level::Fatal:
                    logger->critical("[{}] {}", tag, formattedMsg);
                    break;
                }
            }
        }
    }

    // Implementation of variadic PrintAssertMessage
    template<typename... Args>
    void Log::PrintAssertMessage(Type type, std::string_view prefix, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);

        auto logger = (type == Type::Core) ? s_CoreLogger : s_ClientLogger;
        if (!logger)
            return;

        std::string formattedMsg = std::format(std::forward<Args>(args)...);
        logger->error("{}: {}", prefix, formattedMsg);

#if defined(UT_ASSERT_MESSAGE_BOX) && UT_ASSERT_MESSAGE_BOX && defined(UT_PLATFORM_WINDOWS)
        MessageBoxA(nullptr, formattedMsg.c_str(), "Utopia Assert", MB_OK | MB_ICONERROR);
#endif
    }

} // namespace Utopia

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs                                                                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core logging with tags
#define UT_CORE_TRACE_TAG(tag, ...) ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Trace, (tag), __VA_ARGS__)
#define UT_CORE_INFO_TAG(tag, ...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Info,  (tag), __VA_ARGS__)
#define UT_CORE_WARN_TAG(tag, ...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Warn,  (tag), __VA_ARGS__)
#define UT_CORE_ERROR_TAG(tag, ...) ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Error, (tag), __VA_ARGS__)
#define UT_CORE_FATAL_TAG(tag, ...) ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Fatal, (tag), __VA_ARGS__)

// Client logging with tags
#define UT_TRACE_TAG(tag, ...) ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Trace, (tag), __VA_ARGS__)
#define UT_INFO_TAG(tag, ...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Info,  (tag), __VA_ARGS__)
#define UT_WARN_TAG(tag, ...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Warn,  (tag), __VA_ARGS__)
#define UT_ERROR_TAG(tag, ...) ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Error, (tag), __VA_ARGS__)
#define UT_FATAL_TAG(tag, ...) ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Fatal, (tag), __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Core Logging (without tags)                                                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
#define UT_CORE_TRACE(...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Trace, "CORE", __VA_ARGS__)
#define UT_CORE_INFO(...)   ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Info,  "CORE", __VA_ARGS__)
#define UT_CORE_WARN(...)   ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Warn,  "CORE", __VA_ARGS__)
#define UT_CORE_ERROR(...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Error, "CORE", __VA_ARGS__)
#define UT_CORE_FATAL(...)  ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Core, ::Utopia::Log::Level::Fatal, "CORE", __VA_ARGS__)

// Client Logging without tags
#define UT_TRACE(...)   ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Trace, "CLIENT", __VA_ARGS__)
#define UT_INFO(...)    ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Info,  "CLIENT", __VA_ARGS__)
#define UT_WARN(...)    ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Warn,  "CLIENT", __VA_ARGS__)
#define UT_ERROR(...)   ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Error, "CLIENT", __VA_ARGS__)
#define UT_FATAL(...)   ::Utopia::Log::PrintMessageTag(::Utopia::Log::Type::Client, ::Utopia::Log::Level::Fatal, "CLIENT", __VA_ARGS__)