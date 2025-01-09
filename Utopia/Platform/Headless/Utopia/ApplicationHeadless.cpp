#include "ApplicationHeadless.hpp"

#include "Utopia/Core/Log.hpp"

#include <chrono>
#include <thread>      // For std::this_thread::sleep_for
#include <algorithm>   // For std::min (if you use std::min instead of glm::min)
#include <glm/glm.hpp> // For glm::min<float> if still desired

extern bool g_ApplicationRunning;

static Utopia::Application* s_Instance = nullptr;

namespace Utopia {

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        s_Instance = this;
        Init();
    }

    Application::~Application()
    {
        Shutdown();
        s_Instance = nullptr;
    }

    Application& Application::Get()
    {
        // Make sure s_Instance is valid. You could add an assert here if desired.
        return *s_Instance;
    }

    void Application::Init()
    {
        // Initialize logging (headless mode can still log to console/file)
        Log::Init();
    }

    void Application::Shutdown()
    {
        // Detach and clear all layers
        for (auto& layer : m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();

        // Mark global running state as false
        g_ApplicationRunning = false;

        // Shutdown logging
        Log::Shutdown();
    }

    void Application::Run()
    {
        m_Running = true;

        while (m_Running)
        {
            // Update each layer
            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            // Optional sleep to simulate headless loop without tight CPU usage
            if (m_Specification.SleepDuration > 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(m_Specification.SleepDuration));

            // Calculate new timestep
            float time = GetTime();
            m_FrameTime = time - m_LastFrameTime;
            // You can use std::min if you #include <algorithm>:
            //   m_TimeStep = std::min(m_FrameTime, 0.0333f);
            // or if you rely on GLM:
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;
        }
    }

    void Application::Close()
    {
        m_Running = false;
    }

    float Application::GetTime()
    {
        // Return elapsed time in seconds
        return m_AppTimer.Elapsed();
    }

} // namespace Utopia
