
#include "Animator.h"
#include "MutableSprite.h"
#include "SpriteFramesDrawer.h"

#include "EventHandler/EventHandler.h"
#include "Events/EventFuncFwd.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/MultiGestureEvent.h"
#include "Events/SurfaceChangedEvent.h"

#include "Rendering/ICamera.h"

#include "Math/Vector.h"
#include "Math/Quad.h"
#include "Math/MathFunctions.h"
#include "System/Keycodes.h"

#include "InterfaceDrawer.h"

#include "Rendering/Sprite/ISprite.h"
#include "Rendering/Sprite/SpriteFactory.h"
#include "Rendering/Sprite/AnimationSequence.h"

#include "Rendering/Texture/ITexture.h"
#include "Rendering/Texture/TextureFactory.h"
#include "WriteSpriteFile.h"

#include "../res/animator_sprite_atlas.h"
#include "../res/sprites/add.h"
#include "../res/sprites/delete.h"
#include "../res/sprites/plus.h"
#include "../res/sprites/save.h"

#include <cmath>

using namespace animator;

namespace
{
    void SetupIcons(UIContext& context, std::unordered_map<unsigned int, mono::ITexturePtr>& textures)
    {
        mono::ITexturePtr texture =
            mono::CreateTextureFromData(animator_sprite_atlas_data, animator_sprite_atlas_data_length, "res/animator_sprite_atlas.png");
        textures.insert(std::make_pair(texture->Id(), texture));

        const mono::ISpritePtr add = mono::CreateSpriteFromRaw(add_data);
        const mono::ISpritePtr del = mono::CreateSpriteFromRaw(delete_data);
        const mono::ISpritePtr plus = mono::CreateSpriteFromRaw(plus_data);
        const mono::ISpritePtr save = mono::CreateSpriteFromRaw(save_data);

        context.tools_texture_id = texture->Id();
        context.save_icon = save->GetCurrentFrame().texture_coordinates;
        context.add_icon = add->GetCurrentFrame().texture_coordinates;
        context.plus_icon = plus->GetCurrentFrame().texture_coordinates;
        context.delete_icon = del->GetCurrentFrame().texture_coordinates;
    }
}

Animator::Animator(System::IWindow* window, mono::EventHandler& event_handler, const char* sprite_file)
    : m_window(window)
    , m_event_handler(event_handler)
    , m_sprite_file(sprite_file)
    , m_offset_mode(false)
    , m_offset_highlighted(false)
    , m_moving_offset(false)
{
    using namespace std::placeholders;

    std::unordered_map<unsigned int, mono::ITexturePtr> textures;
    SetupIcons(m_context, textures);
    mono::CreateSprite(m_sprite, sprite_file);

    const System::Size& size = window->Size();
    const math::Vector window_size(size.width, size.height);

    m_gui_renderer = std::make_shared<ImGuiRenderer>(nullptr, window_size, textures);
    m_input_handler = std::make_unique<ImGuiInputHandler>(event_handler);

    const event::KeyDownEventFunc& key_down_func        = std::bind(&Animator::OnDownUp, this, _1);
    const event::MouseDownEventFunc& mouse_down_func    = std::bind(&Animator::OnMouseDown, this, _1);
    const event::MouseUpEventFunc& mouse_up_func        = std::bind(&Animator::OnMouseUp, this, _1);
    const event::MouseMotionEventFunc& mouse_move_func  = std::bind(&Animator::OnMouseMove, this, _1);
    const event::SurfaceChangedEventFunc& surface_func  = std::bind(&Animator::OnSurfaceChanged, this, _1);
    const event::MouseWheelEventFunc& mouse_wheel       = std::bind(&Animator::OnMouseWheel, this, _1);
    const event::MultiGestureEventFunc& multi_gesture   = std::bind(&Animator::OnMultiGesture, this, _1);

    m_key_down_token = event_handler.AddListener(key_down_func);
    m_mouse_down_token = event_handler.AddListener(mouse_down_func);
    m_mouse_up_token = event_handler.AddListener(mouse_up_func);
    m_mouse_move_token = event_handler.AddListener(mouse_move_func);
    m_surface_changed_token = event_handler.AddListener(surface_func);
    m_mouse_wheel_token = event_handler.AddListener(mouse_wheel);
    m_multi_gesture_token = event_handler.AddListener(multi_gesture);

    m_sprite_frame_drawer = std::make_shared<SpriteFramesDrawer>(m_sprite, window_size);
    m_sprite_drawer = std::make_shared<MutableSprite>(m_sprite, m_offset_mode, m_offset_highlighted);

    AddEntity(m_sprite_drawer, 0);
    AddDrawable(m_sprite_frame_drawer, 1);
    AddDrawable(m_gui_renderer, 2);

    AddUpdatable(std::make_shared<InterfaceDrawer>(m_context));

    // Setup UI callbacks
    m_context.toggle_loop           = std::bind(&Animator::OnLoopToggle, this, _1);
    m_context.add_animation         = std::bind(&Animator::OnAddAnimation, this);
    m_context.delete_animation      = std::bind(&Animator::OnDeleteAnimation, this);
    m_context.add_frame             = std::bind(&Animator::OnAddFrame, this);
    m_context.delete_frame          = std::bind(&Animator::OnDeleteFrame, this, _1);
    m_context.set_name              = std::bind(&Animator::OnNameAnimation, this, _1);
    m_context.set_active_animation  = std::bind(&Animator::SetAnimation, this, _1);
    m_context.set_active_frame      = std::bind(&Animator::SetActiveFrame, this, _1);
    m_context.on_save               = std::bind(&Animator::SaveSprite, this);
    
    m_context.max_frames = m_sprite.GetUniqueFrames();
    m_context.selected_frame = 0;

    SetAnimation(m_sprite.GetActiveAnimation());
}

Animator::~Animator()
{
    m_event_handler.RemoveListener(m_key_down_token);
    m_event_handler.RemoveListener(m_mouse_down_token);
    m_event_handler.RemoveListener(m_mouse_up_token);
    m_event_handler.RemoveListener(m_mouse_move_token);
    m_event_handler.RemoveListener(m_surface_changed_token);
    m_event_handler.RemoveListener(m_mouse_wheel_token);
    m_event_handler.RemoveListener(m_multi_gesture_token);
}

void Animator::OnLoad(mono::ICameraPtr& camera)
{
    m_camera = camera;
    camera->SetPosition(math::ZeroVec);
}

int Animator::OnUnload()
{
    m_camera = nullptr;
    SaveSprite();
    return 0;
}

void Animator::SetAnimation(int animation_id)
{
    const int animations = m_sprite.GetDefinedAnimations();
    if(animation_id < animations)
    {
        m_sprite.SetAnimation(animation_id);
        UpdateUIContext(animation_id);
    }
}

void Animator::UpdateUIContext(int animation_id)
{
    mono::AnimationSequence& sequence = m_sprite.GetSequence(animation_id);

    m_context.n_animations = m_sprite.GetDefinedAnimations();
    m_context.animation_id = animation_id;
    m_context.display_name = sequence.GetName();
    m_context.loop_animation = sequence.IsLooping();
    m_context.frames = &sequence.GetFrames();
}

bool Animator::OnDownUp(const event::KeyDownEvent& event)
{
    int animation = -1;
    
    switch(event.key)
    {
        case Keycode::ENTER:
        case Keycode::SPACE:
        {
            m_sprite.RestartAnimation();
            return true;
        }
        case Keycode::LEFT:
        case Keycode::DOWN:
        {
            int id = m_sprite.GetActiveAnimation();
            --id;
            animation = std::max(id, 0);
            break;
        }
        case Keycode::RIGHT:
        case Keycode::UP:
        {
            int id = m_sprite.GetActiveAnimation();
            ++id;
            animation = std::min(id, m_sprite.GetDefinedAnimations() -1);
            break;
        }
        case Keycode::ZERO:
            animation = 0;
            break;
        case Keycode::ONE:
            animation = 1;
            break;
        case Keycode::TWO:
            animation = 2;
            break;
        case Keycode::THREE:
            animation = 3;
            break;
        case Keycode::FOUR:
            animation = 4;
            break;
        case Keycode::FIVE:
            animation = 5;
            break;
        case Keycode::SIX:
            animation = 6;
            break;
        case Keycode::SEVEN:
            animation = 7;
            break;
        case Keycode::EIGHT:
            animation = 8;
            break;
        case Keycode::NINE:
            animation = 9;
            break;
        case Keycode::TAB:
            m_offset_mode = !m_offset_mode;
            break;
        default:
            break;
    }

    if(animation != -1)
    {
        SetAnimation(animation);
        return true;
    }

    return false;
}

bool Animator::IsHooveringZero(const math::Vector& position) const
{
    const System::Size& size = m_window->Size();
    const float picking_distance = m_camera->GetViewport().mB.x / size.width * 5.0f;

    const float distance_to_zero = math::Length(position - math::ZeroVec);
    return  (distance_to_zero < picking_distance);
}

bool Animator::OnMouseDown(const event::MouseDownEvent& event)
{
    if(m_offset_mode)
        m_moving_offset = IsHooveringZero(math::Vector(event.worldX, event.worldY));

    return false;
}

bool Animator::OnMouseUp(const event::MouseUpEvent& event)
{
    if(m_offset_mode)
        m_moving_offset = false;

    return false;
}

bool Animator::OnMouseMove(const event::MouseMotionEvent& event)
{
    if(!m_offset_mode)
        return false;

    const math::Vector mouse_position(event.worldX, event.worldY);
    if(m_moving_offset)
    {
        //const mono::SpriteFrame& current_frame = m_sprite.GetCurrentFrame();
        m_sprite.SetFrameOffset(m_context.selected_frame, mouse_position); // + current_frame.center_offset);
    }
    else
    {
        m_offset_highlighted = IsHooveringZero(mouse_position);
    }

    return false;
}

bool Animator::OnMouseWheel(const event::MouseWheelEvent& event)
{
    const float multiplier = (event.y < 0.0f) ? 1.0f : -1.0f;
    Zoom(multiplier);
    return true;
}

bool Animator::OnMultiGesture(const event::MultiGestureEvent& event)
{
    if(std::fabs(event.distance) < 1e-3)
        return false;

    const float multiplier = (event.distance < 0.0f) ? 1.0f : -1.0f;
    Zoom(multiplier);
    return true;
}

bool Animator::OnSurfaceChanged(const event::SurfaceChangedEvent& event)
{
    if(event.width > 0 && event.height > 0)
        m_gui_renderer->SetWindowSize(math::Vector(event.width, event.height));

    return false;
}

void Animator::OnLoopToggle(bool state)
{
    const int current_id = m_sprite.GetActiveAnimation();
    mono::AnimationSequence& sequence = m_sprite.GetSequence(current_id);
    sequence.SetLooping(state);

    m_sprite.RestartAnimation();
}

void Animator::OnAddAnimation()
{
    const int animation_id = m_sprite.DefineAnimation("new", { 0, 100 }, false);
    SetAnimation(animation_id);
    m_sprite.RestartAnimation();
}

void Animator::OnDeleteAnimation()
{
    const int active_animation = m_sprite.GetActiveAnimation();
    
    std::vector<mono::AnimationSequence>& animations = m_sprite.GetAnimations();
    animations.erase(animations.begin() + active_animation);

    if(active_animation >= static_cast<int>(animations.size()))
        SetAnimation(active_animation -1);
}

void Animator::OnAddFrame()
{
    const int current_id = m_sprite.GetActiveAnimation();
    m_sprite.GetSequence(current_id).AddFrame(0, 100);
}

void Animator::OnDeleteFrame(int id)
{
    const int current_id = m_sprite.GetActiveAnimation();
    m_sprite.GetSequence(current_id).RemoveFrame(id);
}

void Animator::OnNameAnimation(const char* new_name)
{
    const int current_id = m_sprite.GetActiveAnimation();
    m_sprite.GetSequence(current_id).SetName(new_name);
}

void Animator::Zoom(float multiplier)
{
    math::Quad quad = m_camera->GetViewport();

    const float resizeValue = quad.mB.x * 0.15f * multiplier;
    const float aspect = quad.mB.x / quad.mB.y;
    math::ResizeQuad(quad, resizeValue, aspect);

    m_camera->SetTargetViewport(quad);
}

void Animator::SetActiveFrame(int frame)
{
    m_context.selected_frame = frame;
}

void Animator::SaveSprite()
{
    WriteSpriteFile(m_sprite_file, m_sprite.GetAnimations());
}

