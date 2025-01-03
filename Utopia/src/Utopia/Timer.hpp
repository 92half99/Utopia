#pragma once

#include <iostream>
#include <string>
#include <chrono>

namespace Utopia {

    class Timer
    {
    public:
        Timer() noexcept
        {
            Reset();
        }

        void Reset() noexcept
        {
            m_Start = std::chrono::high_resolution_clock::now();
        }

        float Elapsed() const noexcept
        {
            // Changed from multiplying three times by 0.001f to a single factor of 1.0e-9f
            return static_cast<float>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count()) * 1.0e-9f;
        }

        float ElapsedMillis() const noexcept
        {
            return Elapsed() * 1000.0f;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
    };

    class ScopedTimer
    {
    public:
        explicit ScopedTimer(const std::string& name) noexcept
            : m_Name(name)
        {
        }

        ~ScopedTimer() noexcept
        {
            float time = m_Timer.ElapsedMillis();
            std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
        }

    private:
        std::string m_Name;
        Timer m_Timer;
    };

} // namespace Utopia
