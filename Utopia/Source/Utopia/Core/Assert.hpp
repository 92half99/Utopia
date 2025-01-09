#pragma once

#include "Log.hpp"

#ifdef UT_PLATFORM_WINDOWS
    #define UT_DEBUG_BREAK __debugbreak()
#elif defined(UT_COMPILER_CLANG)
    #define UT_DEBUG_BREAK __builtin_debugtrap()
#else
    #define UT_DEBUG_BREAK
#endif

#ifdef UT_DEBUG
    #define UT_ENABLE_ASSERTS
#endif

#define UT_ENABLE_VERIFY

#ifdef UT_ENABLE_ASSERTS
    #ifdef UT_COMPILER_CLANG
        #define UT_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Core, "Assertion Failed", ##__VA_ARGS__)
        #define UT_ASSERT_MESSAGE_INTERNAL(...)       ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Client, "Assertion Failed", ##__VA_ARGS__)
    #else
        #define UT_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Core, "Assertion Failed" __VA_OPT__(,) __VA_ARGS__)
        #define UT_ASSERT_MESSAGE_INTERNAL(...)       ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Client, "Assertion Failed" __VA_OPT__(,) __VA_ARGS__)
    #endif

    #define UT_CORE_ASSERT(condition, ...)  { if(!(condition)) { UT_CORE_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); UT_DEBUG_BREAK; } }
    #define UT_ASSERT(condition, ...)       { if(!(condition)) { UT_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); UT_DEBUG_BREAK; } }
#else
    #define UT_CORE_ASSERT(condition, ...)
    #define UT_ASSERT(condition, ...)
#endif

#ifdef UT_ENABLE_VERIFY
    #ifdef UT_COMPILER_CLANG
        #define UT_CORE_VERIFY_MESSAGE_INTERNAL(...) ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Core, "Verify Failed", ##__VA_ARGS__)
        #define UT_VERIFY_MESSAGE_INTERNAL(...)      ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Client, "Verify Failed", ##__VA_ARGS__)
    #else
        #define UT_CORE_VERIFY_MESSAGE_INTERNAL(...) ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Core, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
        #define UT_VERIFY_MESSAGE_INTERNAL(...)      ::Utopia::Log::PrintAssertMessage(::Utopia::Log::Type::Client, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
    #endif

    #define UT_CORE_VERIFY(condition, ...)  { if(!(condition)) { UT_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); UT_DEBUG_BREAK; } }
    #define UT_VERIFY(condition, ...)       { if(!(condition)) { UT_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); UT_DEBUG_BREAK; } }
#else
    #define UT_CORE_VERIFY(condition, ...)
    #define UT_VERIFY(condition, ...)
#endif
