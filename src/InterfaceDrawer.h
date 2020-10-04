
#pragma once

#include "IUpdatable.h"

namespace animator
{
    class InterfaceDrawer : public mono::IUpdatable
    {
    public:

        InterfaceDrawer(struct UIContext& context);
        void Update(const mono::UpdateContext& update_context) override;
        struct UIContext& m_context;
    };
}
