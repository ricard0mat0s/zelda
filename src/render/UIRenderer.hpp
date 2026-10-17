#pragma once

#include "core/Health.hpp"
#include "raylib.h"

namespace Render {

class UIRenderer {
public:
    UIRenderer() = default;

    // Renders the top-left "- LIFE -" header and heart containers
    void renderHearts(const Core::Health& health);

    // Optional Game Over overlay prompt if dead
    void renderGameOver();
};

} // namespace Render
