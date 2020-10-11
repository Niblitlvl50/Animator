
#include "Animator.h"
#include "SpriteFramesDrawer.h"
#include "SpriteOffsetDrawer.h"

#include "EventHandler/EventHandler.h"
#include "Events/EventFuncFwd.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/MultiGestureEvent.h"
#include "Events/SurfaceChangedEvent.h"
#include "Events/TimeScaleEvent.h"

#include "Camera/ICamera.h"

#include "Math/Vector.h"
#include "Math/Quad.h"
#include "Math/Matrix.h"
#include "Math/MathFunctions.h"
#include "System/Keycodes.h"

#include "InterfaceDrawer.h"

#include "EntitySystem/EntitySystem.h"
#include "Rendering/Sprite/ISprite.h"
#include "Rendering/Sprite/SpriteSystem.h"
#include "Rendering/Sprite/SpriteFactory.h"
#include "Rendering/Sprite/SpriteBatchDrawer.h"
#include "TransformSystem/TransformSystem.h"

#include "Rendering/ImGui.h"
#include "Rendering/Texture/ITexture.h"
#include "Rendering/Texture/ITextureFactory.h"
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
    void SetupIcons(UIContext& context)
    {
        const mono::ITextureFactory* texture_factory = mono::GetTextureFactory();

        mono::ITexturePtr texture =
            texture_factory->CreateTextureFromData(animator_sprite_atlas_data, animator_sprite_atlas_data_length, "res/animator_sprite_atlas.png");
        mono::LoadImGuiTexture(texture);

        const mono::ISpriteFactory* sprite_factory = mono::GetSpriteFactory();

        const mono::ISpritePtr add = sprite_factory->CreateSpriteFromRaw(add_data);
        const mono::ISpritePtr del = sprite_factory->CreateSpriteFromRaw(delete_data);
        const mono::ISpritePtr plus = sprite_factory->CreateSpriteFromRaw(plus_data);
        const mono::ISpritePtr save = sprite_factory->CreateSpriteFromRaw(save_data);

        context.tools_texture_id = texture->Id();
        context.save_icon = save->GetCurrentFrame().texture_coordinates;
        context.add_icon = add->GetCurrentFrame().texture_coordinates;
        context.plus_icon = plus->GetCurrentFrame().texture_coordinates;
        context.delete_icon = del->GetCurrentFrame().texture_coordinates;
    }

    class ActiveFrameUpdater : public mono::IUpdatable
    {
    public:

        ActiveFrameUpdater(const mono::ISprite* sprite, animator::UIContext& ui_context)
            : m_sprite(sprite)
            , m_ui_context(ui_context)
        { }

        void Update(const mono::UpdateContext& update_context) override
        {
            (void)update_context;
            m_ui_context.selected_frame = m_sprite->GetActiveAnimationFrame();
        }
        
        const mono::ISprite* m_sprite;
        animator::UIContext& m_ui_context;
    };
}

Animator::Animator(
    System::IWindow* window,
    mono::TransformSystem* transform_system,
    mono::SpriteSystem* sprite_system,
    mono::EntitySystem* entity_system,
    mono::EventHandler* event_handler,
    float pixels_per_meter,
    const char* sprite_file)
    : m_window(window)
    , m_transform_system(transform_system)
    , m_sprite_system(sprite_system)
    , m_entity_system(entity_system)
    , m_event_handler(event_handler)
    , m_pixels_per_meter(pixels_per_meter)
    , m_sprite_file(sprite_file)
{
    using namespace std::placeholders;

    m_context.update_speed = 1.0f;
    m_context.offset_mode = false;

    // Setup UI callbacks
    m_context.toggle_loop           = std::bind(&Animator::OnLoopToggle, this, _1);
    m_context.add_animation         = std::bind(&Animator::OnAddAnimation, this);
    m_context.delete_animation      = std::bind(&Animator::OnDeleteAnimation, this);
    m_context.add_frame             = std::bind(&Animator::OnAddFrame, this);
    m_context.delete_frame          = std::bind(&Animator::OnDeleteFrame, this, _1);
    m_context.set_name              = std::bind(&Animator::OnNameAnimation, this, _1);
    m_context.set_frame_duration    = std::bind(&Animator::SetFrameDuration, this, _1);
    m_context.set_active_animation  = std::bind(&Animator::SetAnimation, this, _1);
    m_context.set_active_frame      = std::bind(&Animator::SetActiveFrame, this, _1);
    m_context.set_frame_offset      = std::bind(&Animator::SetFrameOffset, this, _1, _2);
    m_context.on_save               = std::bind(&Animator::SaveSprite, this);
    m_context.set_speed             = std::bind(&Animator::SetSpeed, this, _1);
    m_context.toggle_playing        = std::bind(&Animator::TogglePlaying, this);
    m_context.toggle_offset_mode    = std::bind(&Animator::ToggleOffsetMode, this);
}

Animator::~Animator()
{
    m_event_handler->RemoveListener(m_key_down_token);
    m_event_handler->RemoveListener(m_mouse_wheel_token);
    m_event_handler->RemoveListener(m_multi_gesture_token);
}

void Animator::OnLoad(mono::ICamera* camera)
{
    m_camera = camera;

    const math::Quad viewport(0.0f, 0.0f, 10.0f, 7.0f);
    m_camera->SetViewport(viewport);

    SetupIcons(m_context);

    m_sprite_data = const_cast<mono::SpriteData*>(mono::GetSpriteFactory()->GetSpriteDataForFile(m_sprite_file));
    m_context.sprite_data = m_sprite_data;
    m_context.animation_playing = true;

    mono::Entity* entity = m_entity_system->AllocateEntity();

    mono::SpriteComponents sprite_component;
    sprite_component.sprite_file = m_sprite_file;
    m_sprite = m_sprite_system->AllocateSprite(entity->id, sprite_component);

    math::Matrix& sprite_transform = m_transform_system->GetTransform(entity->id);
    math::Position(sprite_transform, math::Center(viewport));

    SetAnimation(m_sprite->GetActiveAnimation());
    //TogglePlaying();

    m_input_handler = std::make_unique<ImGuiInputHandler>(*m_event_handler);

    using namespace std::placeholders;

    const event::KeyDownEventFunc& key_down_func        = std::bind(&Animator::OnDownUp, this, _1);
    const event::MouseWheelEventFunc& mouse_wheel       = std::bind(&Animator::OnMouseWheel, this, _1);
    const event::MultiGestureEventFunc& multi_gesture   = std::bind(&Animator::OnMultiGesture, this, _1);

    m_key_down_token = m_event_handler->AddListener(key_down_func);
    m_mouse_wheel_token = m_event_handler->AddListener(mouse_wheel);
    m_multi_gesture_token = m_event_handler->AddListener(multi_gesture);

    AddDrawable(new mono::SpriteBatchDrawer(m_transform_system, m_sprite_system), 0);
    AddDrawable(new SpriteFramesDrawer(m_sprite_data), 0);
    AddDrawable(new SpriteOffsetDrawer(m_transform_system, m_sprite_system, m_sprite_data, entity->id, m_context.offset_mode), 0);

    AddUpdatable(new ActiveFrameUpdater(m_sprite, m_context));
    AddUpdatable(new InterfaceDrawer(m_context));
}

int Animator::OnUnload()
{
    m_camera = nullptr;
    SaveSprite();
    return 0;
}

void Animator::SetAnimation(int animation_id)
{
    const int animations = m_sprite_data->animations.size();
    if(animation_id < animations)
    {
        m_sprite->SetAnimation(animation_id);
        UpdateUIContext(animation_id);
        SetActiveFrame(m_sprite->GetActiveAnimationFrame());
    }
}

void Animator::UpdateUIContext(int animation_id)
{
    mono::SpriteAnimation& animation = m_sprite_data->animations[animation_id];

    m_context.animation_id = animation_id;
    m_context.animation_name = animation.name.c_str();
    m_context.animation_looping = animation.looping;
    m_context.animation_frame_duration = animation.frame_duration;
}

mono::EventResult Animator::OnDownUp(const event::KeyDownEvent& event)
{
    int animation = -1;
    
    switch(event.key)
    {
        case Keycode::ENTER:
        case Keycode::SPACE:
        {
            TogglePlaying();
            return mono::EventResult::HANDLED;
        }
        case Keycode::LEFT:
        case Keycode::RIGHT:
        {
            const int add_value = (event.key == Keycode::LEFT) ? -1 : +1;
            const int id = m_sprite->GetActiveAnimation() + add_value;
            animation = std::clamp(id, 0, (int)m_sprite_data->animations.size() -1);
            break;
        }
        case Keycode::UP:
        case Keycode::DOWN:
        {
            const int add_value = (event.key == Keycode::UP) ? -1 : +1;
            const int new_active_frame = m_sprite->GetActiveAnimationFrame() + add_value;

            const mono::SpriteAnimation& animation = m_sprite_data->animations[m_sprite->GetActiveAnimation()];
            const int frame = std::clamp(new_active_frame, 0, (int)animation.frames.size() -1);
            SetActiveFrame(frame);

            m_sprite->SetAnimationPlayback(mono::PlaybackMode::PAUSED);
            m_context.animation_playing = false;

            return mono::EventResult::HANDLED;
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
            ToggleOffsetMode();
            break;
        default:
            break;
    }

    if(animation != -1)
    {
        SetAnimation(animation);
        return mono::EventResult::HANDLED;
    }

    return mono::EventResult::PASS_ON;
}

mono::EventResult Animator::OnMouseWheel(const event::MouseWheelEvent& event)
{
    const float multiplier = (event.y < 0.0f) ? 1.0f : -1.0f;
    Zoom(multiplier);
    return mono::EventResult::HANDLED;
}

mono::EventResult Animator::OnMultiGesture(const event::MultiGestureEvent& event)
{
    if(std::fabs(event.distance) < 1e-3)
        return mono::EventResult::PASS_ON;

    const float multiplier = (event.distance < 0.0f) ? 1.0f : -1.0f;
    Zoom(multiplier);
    return mono::EventResult::HANDLED;
}

void Animator::OnLoopToggle(bool state)
{
    const int current_id = m_sprite->GetActiveAnimation();
    m_sprite_data->animations[current_id].looping = state;
    m_sprite->RestartAnimation();
}

void Animator::OnAddAnimation()
{
    mono::SpriteAnimation new_animation;
    new_animation.name = "new";
    new_animation.frames.push_back({ 0, 100 });
    new_animation.looping = false;

    const int animation_id = m_sprite_data->animations.size();
    m_sprite_data->animations.push_back(new_animation);

    SetAnimation(animation_id);
    m_sprite->RestartAnimation();
}

void Animator::OnDeleteAnimation()
{
    const int active_animation = m_sprite->GetActiveAnimation();
    m_sprite_data->animations.erase(m_sprite_data->animations.begin() + active_animation);
    if(active_animation >= static_cast<int>(m_sprite_data->animations.size()))
        SetAnimation(active_animation -1);
}

void Animator::OnAddFrame()
{
    const int current_id = m_sprite->GetActiveAnimation();
    m_sprite_data->animations[current_id].frames.push_back({ 0, 100});
}

void Animator::OnDeleteFrame(int id)
{
    const int current_id = m_sprite->GetActiveAnimation();
    std::vector<mono::SpriteAnimation::Frame>& frames = m_sprite_data->animations[current_id].frames;
    frames.erase(frames.begin() + id);
}

void Animator::OnNameAnimation(const char* new_name)
{
    const int current_id = m_sprite->GetActiveAnimation();
    m_sprite_data->animations[current_id].name = new_name;
}

void Animator::SetFrameDuration(int new_frame_duration)
{
    const int current_id = m_sprite->GetActiveAnimation();
    m_sprite_data->animations[current_id].frame_duration = new_frame_duration;
}

void Animator::Zoom(float multiplier)
{
    math::Quad quad = m_camera->GetViewport();

    const float resize_value = quad.mB.x * 0.15f * multiplier;
    const float aspect = quad.mB.x / quad.mB.y;
    math::ResizeQuad(quad, resize_value, aspect);

    m_camera->SetTargetViewport(quad);
}

void Animator::SetActiveFrame(int frame)
{
    m_sprite->SetActiveAnimationFrame(frame);
    m_context.selected_frame = frame;

    const int active_animation = m_sprite->GetActiveAnimation();
    const int frame_index = m_sprite_data->animations[active_animation].frames[frame].frame;

    m_context.frame_offset_x = m_sprite_data->frames[frame_index].center_offset.x * m_pixels_per_meter;
    m_context.frame_offset_y = m_sprite_data->frames[frame_index].center_offset.y * m_pixels_per_meter;
}

void Animator::SetFrameOffset(int x, int y)
{
    const float x_offset = float(x) / m_pixels_per_meter;
    const float y_offset = float(y) / m_pixels_per_meter;

    const int active_animation = m_sprite->GetActiveAnimation();
    const int active_frame = m_sprite->GetActiveAnimationFrame();
    const int frame_index = m_sprite_data->animations[active_animation].frames[active_frame].frame;
    m_sprite_data->frames[frame_index].center_offset = math::Vector(x_offset, y_offset);
}

void Animator::SaveSprite()
{
    WriteSpriteFile(m_sprite_file, m_sprite_data);
}

void Animator::SetSpeed(float new_speed)
{
    m_event_handler->DispatchEvent(event::TimeScaleEvent(new_speed));
}

void Animator::TogglePlaying()
{
    m_context.animation_playing = !m_context.animation_playing;
    const mono::PlaybackMode new_mode =
        m_context.animation_playing ? mono::PlaybackMode::PLAYING : mono::PlaybackMode::PAUSED;
    m_sprite->SetAnimationPlayback(new_mode);
}

void Animator::ToggleOffsetMode()
{
    m_context.offset_mode = !m_context.offset_mode;
}
