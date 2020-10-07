
#include "SpriteFramesDrawer.h"
#include "Math/Matrix.h"
#include "Math/Quad.h"
#include "Rendering/IRenderer.h"
#include "Rendering/Sprite/SpriteData.h"
#include "Rendering/Sprite/SpriteFactory.h"

#include <algorithm>
#include <numeric>

using namespace animator;

SpriteFramesDrawer::SpriteFramesDrawer(const mono::SpriteData* sprite_data)
    : m_sprite_data(sprite_data)
{
    m_texture = mono::GetTextureFactory()->CreateTexture(sprite_data->texture_file.c_str());
}

void SpriteFramesDrawer::Draw(mono::IRenderer& renderer) const
{
    const float ppm = 2.0f;
    const float spacing = 2.5f / ppm;

    float total_width = 0.0f;
    float max_height = 0.0f;

    for(const auto& frame : m_sprite_data->frames)
    {
        total_width += frame.size.x;
        max_height = std::max(max_height, frame.size.y);
    }

    total_width *= 1.4f;
    total_width /= ppm;
    total_width += ((m_sprite_data->frames.size() +1) * spacing);

    max_height /= ppm;

    const math::Quad& viewport = renderer.GetViewport();
    const float ratio = math::Width(viewport) / math::Height(viewport);

    constexpr math::Matrix identity;
    const math::Matrix& projection = math::Ortho(0.0f, total_width, 0.0f, total_width / ratio, -10.0f, 10.0f);

    const auto transform_scope = mono::MakeTransformScope(identity, &renderer);
    const auto projection_scope = mono::MakeProjectionScope(projection, &renderer);

    math::Vector offset = math::Vector(spacing, max_height / 2.0f);

    for(const auto& frame : m_sprite_data->frames)
    {
        const float half_size = frame.size.x / 2.0f / ppm;
        offset.x += half_size;

        renderer.DrawSprite(frame.texture_coordinates, frame.size, offset, m_texture.get());
        offset.x += (half_size + spacing);
    }
}

math::Quad SpriteFramesDrawer::BoundingBox() const
{
    return math::InfQuad;
}
