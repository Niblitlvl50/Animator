
#pragma once

#include "Rendering/Sprite/SpriteData.h"
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

        bool offset_mode;
        bool animation_playing;
        float update_speed;

        int animation_id;
        const char* animation_name;
        bool animation_looping;
        int animation_frame_duration;

        int selected_frame;
        int frame_offset_x;
        int frame_offset_y;

        mono::SpriteData* sprite_data;

        std::function<void ()> add_animation;
        std::function<void ()> delete_animation;
        std::function<void (int animation_id)> set_active_animation;

        // Active animation callbacks
        std::function<void (const char* new_name)> set_name;
        std::function<void (bool looping)> toggle_loop;
        std::function<void (int new_frame_duration)> set_frame_duration;
        std::function<void ()> add_frame;
        std::function<void (int frame_id)> delete_frame;

        std::function<void (int frame)> set_active_frame;
        std::function<void (int x, int y)> set_frame_offset;
        std::function<void ()> on_save;

        std::function<void ()> toggle_offset_mode;
        std::function<void ()> toggle_playing;
        std::function<void (float update_speed)> set_speed;
    };
}
