
#include "SpriteFramesDrawer.h"
#include "Math/Matrix.h"
#include "Math/Quad.h"
#include "Rendering/IRenderer.h"
#include "Rendering/Sprite/Sprite.h"

#include <algorithm>
#include <numeric>

using namespace animator;

SpriteFramesDrawer::SpriteFramesDrawer(mono::Sprite& sprite, const math::Vector& window_size)
    : m_sprite(sprite)
    , m_window_size(window_size)
{ }

void SpriteFramesDrawer::Draw(mono::IRenderer& renderer) const
{
    /*
    const float ppm = 2.0f; //renderer.PixelsPerMeter();
    const float spacing = 2.5f / ppm;

    float total_width = 0.0f;
    float max_height = 0.0f;

    for(int index = 0; index < m_sprite.GetUniqueFrames(); ++index)
    {
        const mono::SpriteFrame& sprite_frame = m_sprite.GetFrame(index);
        total_width += sprite_frame.size.x;
        max_height = std::max(max_height, sprite_frame.size.y);
    }

    total_width *= 1.4f;
    total_width /= ppm;
    total_width += ((m_sprite.GetUniqueFrames() +1) * spacing);

    max_height /= ppm;

    const float ratio = m_window_size.x / m_window_size.y;

    constexpr math::Matrix identity;
    const math::Matrix& projection = math::Ortho(0.0f, total_width, 0.0f, total_width / ratio, -10.0f, 10.0f);
    renderer.PushNewTransform(identity);
    renderer.PushNewProjection(projection);

    math::Vector offset = math::Vector(spacing, max_height / 2.0f);

    for(int frame_index = 0; frame_index < m_sprite.GetUniqueFrames(); ++frame_index)
    {
        const mono::SpriteFrame& sprite_frame = m_sprite.GetFrame(frame_index);
        const float half_size = sprite_frame.size.x / 2.0f / ppm;

        offset.x += half_size;
        renderer.DrawSprite(sprite_frame.texture_coordinates, sprite_frame.size, offset, m_sprite.GetTexture().get());

        const std::string& number = std::to_string(frame_index);
        constexpr mono::Color::RGBA text_color(0.0f, 0.8f, 1.0f);
        renderer.DrawText(0, number.c_str(), offset, true, text_color);

        offset.x += (half_size + spacing);
    }
    */
}

math::Quad SpriteFramesDrawer::BoundingBox() const
{
    return math::InfQuad;
}
