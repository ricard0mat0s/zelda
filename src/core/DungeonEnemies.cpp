#include "core/DungeonEnemies.hpp"
#include "core/Dungeon.hpp"
#include <cmath>
#include <algorithm>

namespace Core {

// ==================== BAT (KEESE) ====================

Bat::Bat(float x, float y)
    : m_pos(x, y), m_basePos(x, y) {}

void Bat::update(float dt, const Vec2& playerPos) {
    if (m_isDead) return;

    if (m_hurtTimer > 0.0f) {
        m_hurtTimer -= dt;
    }

    // Wing flapping animation
    m_animTimer += dt;
    if (m_animTimer >= 0.12f) {
        m_animTimer -= 0.12f;
        m_animFrame = (m_animFrame + 1) % 2;
    }

    m_stateTimer += dt;

    float dx = playerPos.x - m_pos.x;
    float dy = playerPos.y - m_pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // AI: Swoop towards player if within 130px, else hover in sine pattern
    if (dist < 130.0f) {
        m_swooping = true;
    } else if (dist > 180.0f) {
        m_swooping = false;
    }

    if (m_swooping && dist > 1.0f) {
        Vec2 dir = Vec2(dx / dist, dy / dist);
        // Add subtle wave oscillation
        float wave = std::sin(m_stateTimer * 8.0f) * 20.0f;
        Vec2 perp = Vec2(-dir.y, dir.x);
        m_pos += (dir * 85.0f + perp * wave) * dt;
    } else {
        // Gentle figure-8 hovering
        m_pos.x = m_basePos.x + std::cos(m_stateTimer * 2.0f) * 24.0f;
        m_pos.y = m_basePos.y + std::sin(m_stateTimer * 4.0f) * 12.0f;
    }

    // Clamp inside room
    m_pos.x = std::clamp(m_pos.x, 20.0f, DungeonRoom::ROOM_WIDTH - 36.0f);
    m_pos.y = std::clamp(m_pos.y, 20.0f, DungeonRoom::ROOM_HEIGHT - 36.0f);
}

bool Bat::takeDamage(int amount) {
    if (m_isDead || m_hurtTimer > 0.0f) return false;

    m_hp -= amount;
    m_hurtTimer = 0.2f;
    if (m_hp <= 0) {
        m_isDead = true;
    }
    return true;
}

std::string Bat::getTextureName() const {
    return "bat_" + std::to_string(m_animFrame);
}


// ==================== SKELETON (STALFOS) ====================

Skeleton::Skeleton(float x, float y)
    : m_pos(x, y) {}

void Skeleton::update(float dt, const Vec2& playerPos, const Dungeon& dungeon) {
    if (m_isDead) return;

    if (m_hurtTimer > 0.0f) {
        m_hurtTimer -= dt;
    }

    // Decay knockback
    if (m_knockback.length() > 0.001f) {
        m_pos = dungeon.resolveMovement(m_pos, m_knockback * dt, m_localHitbox);
        m_knockback -= m_knockback * std::min(1.0f, dt * 8.0f);
        if (m_knockback.length() < 5.0f) {
            m_knockback = Vec2{0.0f, 0.0f};
        }
        return; // Knockback overrides walking
    }

    float dx = playerPos.x - m_pos.x;
    float dy = playerPos.y - m_pos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    Vec2 displacement{0.0f, 0.0f};

    if (dist < 140.0f && dist > 1.0f) {
        // Chase player
        Vec2 dir = Vec2(dx / dist, dy / dist);
        displacement = dir * m_speed * dt;

        if (std::abs(dx) > std::abs(dy)) {
            m_facing = (dx > 0) ? Direction::Right : Direction::Left;
        } else {
            m_facing = (dy > 0) ? Direction::Down : Direction::Up;
        }

        m_animTimer += dt;
        if (m_animTimer >= 0.18f) {
            m_animTimer -= 0.18f;
            m_animFrame = (m_animFrame + 1) % 2;
        }
    } else {
        // Idle/patrol step
        m_patrolTimer += dt;
        if (m_patrolTimer >= 2.0f) {
            m_patrolTimer = 0.0f;
            m_animFrame = 0;
        }
    }

    if (displacement.length() > 0.0001f) {
        m_pos = dungeon.resolveMovement(m_pos, displacement, m_localHitbox);
    }
}

bool Skeleton::takeDamage(int amount, const Vec2& knockbackVelocity) {
    if (m_isDead || m_hurtTimer > 0.0f) return false;

    m_hp -= amount;
    m_hurtTimer = 0.3f;
    m_knockback = knockbackVelocity;

    if (m_hp <= 0) {
        m_isDead = true;
    }
    return true;
}

std::string Skeleton::getTextureName() const {
    return "skeleton_" + std::to_string(m_animFrame);
}

} // namespace Core
