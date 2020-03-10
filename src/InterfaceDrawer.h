
#pragma once

#include "IUpdatable.h"

namespace animator
{
    struct UIContext;

    class InterfaceDrawer : public mono::IUpdatable
    {
    public:

        InterfaceDrawer(UIContext& context);
        void doUpdate(const mono::UpdateContext& update_context) override;

    private:

        UIContext& m_context;
    };
}
