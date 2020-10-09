
#pragma once

#include "System/System.h"
#include "MonoFwd.h"
#include "Zone/ZoneBase.h"
#include "EventHandler/EventToken.h"
#include "Events/EventFwd.h"
#include "UIContext.h"

#include "ImGuiImpl/ImGuiInputHandler.h"

#include <memory>

namespace animator
{
    class Animator : public mono::ZoneBase
    {
    public:

        Animator(
            System::IWindow* window,
            mono::TransformSystem* transform_system,
            mono::SpriteSystem* sprite_system,
            mono::EntitySystem* entity_system,
            mono::EventHandler* event_handler,
            const char* sprite_file);
        ~Animator();

        void OnLoad(mono::ICamera* camera) override;
        int OnUnload() override;

        void SetAnimation(int animation_id);
        void UpdateUIContext(int animation_id);

        mono::EventResult OnDownUp(const event::KeyDownEvent& event);
        mono::EventResult OnMouseDown(const event::MouseDownEvent& event);
        mono::EventResult OnMouseUp(const event::MouseUpEvent& event);
        mono::EventResult OnMouseMove(const event::MouseMotionEvent& event);
        mono::EventResult OnMouseWheel(const event::MouseWheelEvent& event);
        mono::EventResult OnMultiGesture(const event::MultiGestureEvent& event);

        void OnLoopToggle(bool state);
        void OnAddAnimation();
        void OnDeleteAnimation();
        void OnAddFrame();
        void OnDeleteFrame(int id);
        void OnNameAnimation(const char* new_name);
        void SetFrameRate(int new_frame_rate);
        void SetActiveFrame(int frame);

        void Zoom(float multiplier);
        void SaveSprite();
        void SetSpeed(float new_speed);
        void TogglePlaying();
        void ToggleOffsetMode();

    private:

        bool IsHooveringZero(const math::Vector& position) const;

        System::IWindow* m_window;
        mono::TransformSystem* m_transform_system;
        mono::SpriteSystem* m_sprite_system;
        mono::EntitySystem* m_entity_system;
        mono::EventHandler* m_event_handler;
        const char* m_sprite_file;

        mono::EventToken<event::KeyDownEvent> m_key_down_token;
        mono::EventToken<event::MouseDownEvent> m_mouse_down_token;
        mono::EventToken<event::MouseUpEvent> m_mouse_up_token;
        mono::EventToken<event::MouseMotionEvent> m_mouse_move_token;
        mono::EventToken<event::MouseWheelEvent> m_mouse_wheel_token;
        mono::EventToken<event::MultiGestureEvent> m_multi_gesture_token;

        std::unique_ptr<ImGuiInputHandler> m_input_handler;

        UIContext m_context;

        mono::ICamera* m_camera;
        mono::SpriteData* m_sprite_data;
        mono::ISprite* m_sprite;

        bool m_offset_mode;
        bool m_offset_highlighted;
        bool m_moving_offset;
    };
}
