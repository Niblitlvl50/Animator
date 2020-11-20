
#include "System/System.h"
#include "System/File.h"
#include "System/UID.h"

#include "SystemContext.h"
#include "Camera/Camera.h"
#include "EntitySystem/EntitySystem.h"
#include "Rendering/RenderSystem.h"
#include "Rendering/Sprite/SpriteSystem.h"
#include "Rendering/Text/TextFunctions.h"
#include "TransformSystem/TransformSystem.h"

#include "EventHandler/EventHandler.h"
#include "Engine.h"

#include "Animator.h"

#include "../res/pixelette.h"

#include <cstdio>

int main(int argc, const char* argv[])
{
    if(argc < 2)
    {
        std::printf("You need to supply an argument\n");
        return 0;
    }

    // This is assumed to be the file argument
    const char* sprite_file = argv[1];
    if(!file::Exists(sprite_file))
    {
        std::printf("Unable to find the file '%s'.\n", sprite_file);
        return 0;
    }

    constexpr size_t max_entities = 500;

    System::InitializeContext init_context;
    init_context.working_directory = ".";
    System::Initialize(init_context);
    System::SetUIDOffset(max_entities +1);

    mono::RenderInitParams render_params;
    mono::InitializeRender(render_params);

    {
        mono::EventHandler event_handler;
        System::IWindow* window = System::CreateWindow("Animator", 0, 0, 1200, 800, System::WindowOptions::NONE);

        mono::LoadFontRaw(0, pixelette_data, pixelette_data_length, 48.0f, 0.01f);
        mono::LoadFontRaw(1, pixelette_data, pixelette_data_length, 48.0f, 0.05f);

        mono::SystemContext system_context;
        mono::TransformSystem* transform_system = system_context.CreateSystem<mono::TransformSystem>(max_entities);
        mono::SpriteSystem* sprite_system = system_context.CreateSystem<mono::SpriteSystem>(max_entities, transform_system);
        mono::EntitySystem* entity_system = system_context.CreateSystem<mono::EntitySystem>(max_entities);

        mono::Camera camera;
        mono::Engine engine(window, &camera, &system_context, &event_handler);

        animator::Animator animator(
            transform_system, sprite_system, entity_system, &event_handler, render_params.pixels_per_meter, sprite_file);
        engine.Run(&animator);

        delete window;
    }

    mono::ShutdownRender();
    System::Shutdown();
    
    return 0;
}
