#pragma once

#include "core/Health.hpp"
#include "core/Inventory.hpp"
#include "raylib.h"

namespace Render {

class UIRenderer {
public:
    UIRenderer() = default;

    // Renders the top-left "- LIFE -" header and heart containers
    void renderHearts(const Core::Health& health);

    // Renders equipped weapons and keys in top-right HUD
    void renderInventory(const Core::Inventory& inventory);

    // Optional Game Over overlay prompt if dead
    void renderGameOver();
};

} // namespace Render
