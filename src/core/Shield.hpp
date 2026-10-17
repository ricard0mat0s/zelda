#pragma once

#include "core/types.hpp"

namespace Core {

// Determines if an incoming threat at attackerPos is deflected by the player's frontal shield
// Uses a 120-degree frontal cone (cos(theta) >= 0.5) based on the player's facing direction
bool isAttackBlockedByShield(const Vec2& playerPos, Direction playerFacing, const Vec2& attackerPos);

} // namespace Core
