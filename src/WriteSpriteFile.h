
#pragma once

#include "Rendering/Sprite/SpriteData.h"
#include <vector>

namespace animator
{
    void WriteSpriteFile(const char* sprite_file, const std::vector<mono::SpriteAnimation>& animations);
}
