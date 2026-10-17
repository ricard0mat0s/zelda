#include "core/Shield.hpp"
#include <cmath>

namespace Core {

bool isAttackBlockedByShield(const Vec2& playerPos, Direction playerFacing, const Vec2& attackerPos) {
    float dx = attackerPos.x - playerPos.x;
    float dy = attackerPos.y - playerPos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.0001f) {
        return false; // Direct overlap cannot be shielded
    }

    Vec2 facingNormal = Vec2::fromDirection(playerFacing);
    float dot = (dx * facingNormal.x + dy * facingNormal.y) / dist;

    // Frontal 120-degree cone: cos(60 deg) = 0.5
    return dot >= 0.5f;
}

} // namespace Core
