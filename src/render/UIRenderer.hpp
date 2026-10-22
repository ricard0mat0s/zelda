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

    // Renders dynamic boss health bar at top-center of screen
    void renderBossHealthBar(const char* name, int currentHP, int maxHP);

    // Optional Game Over overlay prompt if dead
    void renderGameOver();

    // Victory screen overlay upon collecting the Boss Heart Container
    void renderVictoryScreen(int maxHearts);
};

} // namespace Render
