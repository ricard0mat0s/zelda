#include "core/Combat.hpp"
#include <cmath>

namespace Core {

Rect getSwordHitbox(const Vec2& playerPos, Direction facing) {
    switch (facing) {
        case Direction::Down:
            return Rect(playerPos.x, playerPos.y + 14.0f, 16.0f, 14.0f);
        case Direction::Up:
            return Rect(playerPos.x, playerPos.y - 14.0f, 16.0f, 14.0f);
        case Direction::Left:
            return Rect(playerPos.x - 14.0f, playerPos.y, 14.0f, 16.0f);
        case Direction::Right:
            return Rect(playerPos.x + 16.0f, playerPos.y, 14.0f, 16.0f);
    }
    return Rect(playerPos.x, playerPos.y, 16.0f, 16.0f);
}

Vec2 calculateKnockback(const Vec2& attackerPos, const Vec2& targetPos, float force) {
    float dx = targetPos.x - attackerPos.x;
    float dy = targetPos.y - attackerPos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.0001f) {
        return {0.0f, force}; // Default downward impulse if overlapping
    }

    return { (dx / dist) * force, (dy / dist) * force };
}

bool canPerformAttack(bool hasWeapon, bool isAttacking, float cooldownRemaining) {
    return hasWeapon && !isAttacking && (cooldownRemaining <= 0.0f);
}

} // namespace Core
