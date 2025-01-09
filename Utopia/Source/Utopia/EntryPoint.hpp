#pragma once

#include <memory>

extern Utopia::Application* Utopia::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

inline void StopApplicationLoop() noexcept
{
    g_ApplicationRunning = false;
}

namespace Utopia {

    inline int Main(int argc, char** argv)
    {
        while (g_ApplicationRunning)
        {
            std::unique_ptr<Application> app(CreateApplication(argc, argv));
            app->Run();
        }

        return 0;
    }

} // namespace Utopia

#if defined(UT_PLATFORM_WINDOWS) && !defined(UT_HEADLESS) && defined(UT_DIST)

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE /*hInst*/, HINSTANCE /*hInstPrev*/, PSTR /*cmdline*/, int /*cmdshow*/)
{
    return Utopia::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
    return Utopia::Main(argc, argv);
}

#endif // defined(UT_PLATFORM_WINDOWS) && defined(UT_DIST)
