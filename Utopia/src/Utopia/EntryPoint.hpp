#pragma once

#ifdef WL_PLATFORM_WINDOWS

#include <memory>

extern Utopia::Application* Utopia::CreateApplication(int argc, char** argv);
bool g_ApplicationRunning = true;

namespace Utopia {

    int Main(int argc, char** argv)
    {
        while (g_ApplicationRunning)
        {
            auto app = std::unique_ptr<Application>(CreateApplication(argc, argv));
            app->Run();
        }

        return 0;
    }

} // namespace Utopia

#ifdef WL_DIST

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

#endif // WL_DIST

#endif // WL_PLATFORM_WINDOWS
