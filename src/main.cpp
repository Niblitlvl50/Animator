
#include "System/System.h"
#include "Rendering/RenderSystem.h"

#include "Camera/TraceCamera.h"
#include "EventHandler/EventHandler.h"
#include "Rendering/Text/TextFunctions.h"

#include "Engine.h"
#include "Animator.h"

#include "../res/pixelette.h"

#include <cstdio>

int main(int argc, const char * argv[])
{
    if(argc < 2)
    {
        std::printf("You need to supply an argument\n");
        return 1;
    }

    // This is assumed to be the file argument
    const char* file = argv[1];

    System::Initialize();
    mono::InitializeRender();

    {
        mono::EventHandler event_handler;

        System::IWindow* window = System::CreateWindow("Animator", 0, 0, 1200, 800, false);
        window->SetBackgroundColor(0.6, 0.6, 0.6);

        mono::ICameraPtr camera = std::make_shared<mono::TraceCamera>(12, 8);
        mono::LoadFontRaw(0, pixelette_data, pixelette_data_length, 48.0f, 0.01f);

        mono::Engine engine(window, camera, event_handler);
        engine.Run(std::make_shared<animator::Animator>(window, event_handler, file));

        delete window;
    }

    mono::ShutdownRender();
    System::Shutdown();
    
    return 0;
}
