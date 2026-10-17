#pragma once

#include "core/types.hpp"

namespace Core {

// Calculates directional sword slash hitbox in front of player
Rect getSwordHitbox(const Vec2& playerPos, Direction facing);

// Calculates knockback vector directed away from attacker
Vec2 calculateKnockback(const Vec2& attackerPos, const Vec2& targetPos, float force = 140.0f);

// Validates attack feasibility based on weapon ownership and cooldowns
bool canPerformAttack(bool hasWeapon, bool isAttacking, float cooldownRemaining);

} // namespace Core
