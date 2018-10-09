
#pragma once

#include "Math/Vector.h"
#include "Rendering/IDrawable.h"

namespace mono
{
    class Sprite;
}

namespace animator
{
    class SpriteFramesDrawer : public mono::IDrawable
    {
    public:

        SpriteFramesDrawer(mono::Sprite& sprite, const math::Vector& window_size);

        void doDraw(mono::IRenderer& renderer) const override;
        math::Quad BoundingBox() const override;

        const mono::Sprite& m_sprite;
        const math::Vector m_window_size;
    };
}
