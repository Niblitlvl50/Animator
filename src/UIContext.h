
#pragma once

#include "Rendering/Sprite/AnimationSequence.h"
#include "Math/Quad.h"
#include <vector>
#include <functional>

namespace animator
{
    struct UIContext
    {
        int tools_texture_id;
        math::Quad save_icon;
        math::Quad add_icon;
        math::Quad plus_icon;
        math::Quad delete_icon;

        int animation_id;
        int n_animations;
        int max_frames;
        int selected_frame;
        
        const char* display_name;
        bool loop_animation;
        std::vector<mono::Frame>* frames;

        std::function<void ()> add_animation;
        std::function<void ()> delete_animation;
        std::function<void (int)> set_active_animation;

        // Active animation callbacks
        std::function<void (const char*)> set_name;
        std::function<void (bool)> toggle_loop;
        std::function<void ()> add_frame;
        std::function<void (int)> delete_frame;

        std::function<void (int)> set_active_frame;

        std::function<void ()> on_save;
    };
}
