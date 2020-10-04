
#include "MutableSprite.h"
#include "Rendering/Sprite/ISprite.h"
#include "Rendering/IRenderer.h"
#include "Rendering/Color.h"

using namespace animator;

MutableSprite::MutableSprite(mono::ISprite& sprite, bool& draw_cross, bool& highlighted)
    : m_sprite(sprite)
    , m_draw_cross(draw_cross)
    , m_highlighted_cross(highlighted)
{ }

void MutableSprite::EntityDraw(mono::IRenderer& renderer) const
{
    constexpr mono::Color::RGBA default_color(1.0f, 0.0f, 0.0f);
    constexpr mono::Color::RGBA color_highlighted(0.0f, 0.5f, 1.0f, 1.0f);

    renderer.DrawSprite(m_sprite);

    if(m_draw_cross)
    {
        const mono::Color::RGBA& color = m_highlighted_cross ? color_highlighted : default_color;
        const std::vector<math::Vector>& cross = {
            math::Vector(-0.2f, 0.0f),
            math::Vector(0.2f, 0.0f),
            math::Vector(0.0f, -0.2f),
            math::Vector(0.0f, 0.2f)
        };

        renderer.DrawLines(cross, color, 2.0f);
        renderer.DrawPoints({ math::ZeroVec }, color, 12.0f);
    }
}

void MutableSprite::EntityUpdate(const mono::UpdateContext& update_context)
{
    m_sprite.Update(update_context);
}
