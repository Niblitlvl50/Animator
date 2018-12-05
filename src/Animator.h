
#pragma once

#include "System/System.h"
#include "MonoFwd.h"
#include "Zone/ZoneBase.h"
#include "EventHandler/EventToken.h"
#include "Events/EventFwd.h"
#include "UIContext.h"
#include "Rendering/Sprite/Sprite.h"

#include "ImGuiImpl/ImGuiInputHandler.h"
#include "ImGuiImpl/ImGuiRenderer.h"

#include <memory>

namespace animator
{
    class Animator : public mono::ZoneBase
    {
    public:

        Animator(System::IWindow* window, mono::EventHandler& event_handler, const char* sprite_file);
        ~Animator();

        virtual void OnLoad(mono::ICameraPtr& camera);
        virtual int OnUnload();

        void SetAnimation(int animation_id);
        void UpdateUIContext(int animation_id);

        bool OnDownUp(const event::KeyDownEvent& event);
        bool OnMouseDown(const event::MouseDownEvent& event);
        bool OnMouseUp(const event::MouseUpEvent& event);
        bool OnMouseMove(const event::MouseMotionEvent& event);
        bool OnMouseWheel(const event::MouseWheelEvent& event);
        bool OnMultiGesture(const event::MultiGestureEvent& event);
        bool OnSurfaceChanged(const event::SurfaceChangedEvent& event);
        
        void OnLoopToggle(bool state);
        void OnAddAnimation();
        void OnDeleteAnimation();
        void OnAddFrame();
        void OnDeleteFrame(int id);
        void OnNameAnimation(const char* new_name);
        void SetActiveFrame(int frame);

        void Zoom(float multiplier);
        void SaveSprite();

    private:

        bool IsHooveringZero(const math::Vector& position) const;

        System::IWindow* m_window;
        mono::EventHandler& m_event_handler;
        const char* m_sprite_file;

        mono::EventToken<event::KeyDownEvent> m_key_down_token;
        mono::EventToken<event::MouseDownEvent> m_mouse_down_token;
        mono::EventToken<event::MouseUpEvent> m_mouse_up_token;
        mono::EventToken<event::MouseMotionEvent> m_mouse_move_token;
        mono::EventToken<event::SurfaceChangedEvent> m_surface_changed_token;
        mono::EventToken<event::MouseWheelEvent> m_mouse_wheel_token;
        mono::EventToken<event::MultiGestureEvent> m_multi_gesture_token;
        
        std::shared_ptr<class SpriteFramesDrawer> m_sprite_frame_drawer;
        std::shared_ptr<class MutableSprite> m_sprite_drawer;
        std::shared_ptr<ImGuiRenderer> m_gui_renderer;
        std::unique_ptr<ImGuiInputHandler> m_input_handler;

        UIContext m_context;

        mono::ICameraPtr m_camera;
        mono::Sprite m_sprite;
        bool m_offset_mode;
        bool m_offset_highlighted;
        bool m_moving_offset;
    };
}
