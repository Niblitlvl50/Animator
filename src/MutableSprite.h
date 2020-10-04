
#pragma once

#include "Zone/EntityBase.h"
#include "Rendering/RenderPtrFwd.h"

namespace animator
{
    class MutableSprite : public mono::EntityBase
    {
    public:

        MutableSprite(mono::ISprite& sprite, bool& draw_cross, bool& highlighted);

        void EntityDraw(mono::IRenderer& renderer) const override;
        void EntityUpdate(const mono::UpdateContext& update_context) override;

    private:

        mono::ISprite& m_sprite;
        const bool& m_draw_cross;
        const bool& m_highlighted_cross;
    };
}
