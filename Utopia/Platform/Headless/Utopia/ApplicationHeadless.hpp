#pragma once

#include "Utopia/Layer.hpp"
#include "Utopia/Timer.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace Utopia {

    struct ApplicationSpecification
    {
        std::string Name = "Utopia App";
        uint32_t Width = 1600;
        uint32_t Height = 900;

        // Time in milliseconds to sleep each frame (simulates no render loop).
        uint64_t SleepDuration = 0;
    };

    class Application
    {
    public:
        explicit Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
        ~Application();

        // Singleton access
        static Application& Get();

        // Primary run loop for headless mode
        void Run();

        // No menubar for headless apps, so this is a no-op
        inline void SetMenubarCallback(const std::function<void()>&) {}

        template<typename T>
        void PushLayer()
        {
            static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
            auto layer = std::make_shared<T>();
            layer->OnAttach();
            m_LayerStack.emplace_back(layer);
        }

        void PushLayer(const std::shared_ptr<Layer>& layer)
        {
            m_LayerStack.emplace_back(layer);
            layer->OnAttach();
        }

        // Exits the Run() loop
        void Close();

        // Returns elapsed time (in seconds) since the application started
        float GetTime();

    private:
        void Init();
        void Shutdown();

    private:
        ApplicationSpecification m_Specification;
        bool m_Running = false;

        float m_TimeStep    = 0.0f;
        float m_FrameTime   = 0.0f;
        float m_LastFrameTime = 0.0f;

        std::vector<std::shared_ptr<Layer>> m_LayerStack;
        Timer m_AppTimer;
    };

    // Implemented by the client (the user of this framework)
    Application* CreateApplication(int argc, char** argv);

} // namespace Utopia
