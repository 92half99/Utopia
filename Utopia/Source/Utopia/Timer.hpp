#pragma once

#include <chrono>
#include <iostream>
#include <string>

namespace Utopia {

    class Timer
    {
    public:
        Timer()
        {
            Reset();
        }

        void Reset() noexcept
        {
            m_Start = std::chrono::steady_clock::now();
        }
        
        float Elapsed() const
        {
            const auto now = std::chrono::steady_clock::now();
            const auto duration = std::chrono::duration<float>(now - m_Start);
            return duration.count();
        }

        float ElapsedMillis() const
        {
            return Elapsed() * 1000.0f;
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> m_Start;
    };

    class ScopedTimer
    {
    public:
        explicit ScopedTimer(const std::string& name)
            : m_Name(name)
        {
        }

        ~ScopedTimer() noexcept
        {
            const float time = m_Timer.ElapsedMillis();
            std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
        }

    private:
        std::string m_Name;
        Timer m_Timer;
    };

} // namespace Utopia
