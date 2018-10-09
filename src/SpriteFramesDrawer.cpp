
#include "SpriteFramesDrawer.h"
#include "Math/Matrix.h"
#include "Math/Quad.h"
#include "Rendering/IRenderer.h"
#include "Rendering/Sprite/Sprite.h"

using namespace animator;

SpriteFramesDrawer::SpriteFramesDrawer(mono::Sprite& sprite, const math::Vector& window_size)
    : m_sprite(sprite)
    , m_window_size(window_size)
{ }

void SpriteFramesDrawer::doDraw(mono::IRenderer& renderer) const
{
    constexpr math::Matrix identity;
    const math::Matrix& projection = math::Ortho(0.0f, 20.0f, 0.0f, 15.0f, -10.0f, 10.0f);

    renderer.PushNewTransform(identity);
    renderer.PushNewProjection(projection);

    constexpr mono::Color::RGBA text_color(0.0f, 0.8f, 1.0f);
    math::Vector offset(1, 1);

    for(int frame_index = 0; frame_index < m_sprite.GetUniqueFrames(); ++frame_index)
    {
        const math::Quad& frame_quad = m_sprite.GetFrame(frame_index);
        renderer.DrawSprite(frame_quad, offset, m_sprite.GetTexture());

        const std::string number = std::to_string(frame_index);
        renderer.DrawText(0, number.c_str(), offset + math::Vector(0.0f, -0.75f), true, text_color);

        offset += math::Vector(1.1f, 0.0f);
    }
}

math::Quad SpriteFramesDrawer::BoundingBox() const
{
    return math::InfQuad;
}
