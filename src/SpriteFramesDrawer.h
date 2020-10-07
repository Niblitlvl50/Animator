
#pragma once

#include "Rendering/IDrawable.h"
#include "Rendering/Texture/ITextureFactory.h"

namespace mono
{
    struct SpriteData;
}

namespace animator
{
    class SpriteFramesDrawer : public mono::IDrawable
    {
    public:

        SpriteFramesDrawer(const mono::SpriteData* sprite_data);

        void Draw(mono::IRenderer& renderer) const override;
        math::Quad BoundingBox() const override;

        const mono::SpriteData* m_sprite_data;
        mono::ITexturePtr m_texture;
    };
}
