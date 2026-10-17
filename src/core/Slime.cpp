#include "core/Slime.hpp"
#include <cmath>
#include <algorithm>

namespace Core {

Slime::Slime(float x, float y)
    : m_pos(x, y) {}

bool Slime::takeDamage(int amount, const Vec2& knockbackVelocity) {
    if (m_isDead || m_hurtTimer > 0.0f || amount <= 0) {
        return false;
    }

    m_hp -= amount;
    m_hurtTimer = 0.25f;
    m_knockback = knockbackVelocity;
    m_isHopping = false;
    m_hopTimer = 0.0f;

    if (m_hp <= 0) {
        m_hp = 0;
        m_isDead = true;
    }

    return true;
}

void Slime::update(float dt, const Vec2& playerPos, const MapData& map) {
    if (m_isDead) return;

    if (m_hurtTimer > 0.0f) {
        m_hurtTimer = std::max(0.0f, m_hurtTimer - dt);
    }

    // 1. Process knockback if active
    if (m_knockback.length() > 5.0f) {
        Vec2 disp = m_knockback * dt;
        m_pos = map.resolveMovement(m_pos, disp, m_localHitbox);
        m_knockback = m_knockback * std::max(0.0f, 1.0f - dt * 10.0f);
        m_animFrame = 0;
        return;
    }

    // 2. Hopping physics & state
    if (m_isHopping) {
        m_hopTimer -= dt;
        Vec2 disp = m_hopVelocity * dt;
        m_pos = map.resolveMovement(m_pos, disp, m_localHitbox);
        m_animFrame = 1;

        if (m_hopTimer <= 0.0f) {
            m_isHopping = false;
            m_animFrame = 0;
            m_hopCooldown = 0.7f;
        }
    } else {
        m_animFrame = 0;
        m_hopCooldown -= dt;

        if (m_hopCooldown <= 0.0f) {
            float dx = (playerPos.x + 8.0f) - (m_pos.x + 8.0f);
            float dy = (playerPos.y + 8.0f) - (m_pos.y + 8.0f);
            float dist = std::sqrt(dx * dx + dy * dy);

            constexpr float DETECTION_RANGE = 120.0f;
            constexpr float HOP_SPEED = 55.0f;

            if (dist <= DETECTION_RANGE && dist > 1.0f) {
                // Leap towards player
                m_hopVelocity = Vec2(dx / dist * HOP_SPEED, dy / dist * HOP_SPEED);
            } else {
                // Idle wander: hop based on position parity
                int seed = static_cast<int>(m_pos.x * 7 + m_pos.y * 13) % 4;
                switch (seed) {
                    case 0: m_hopVelocity = Vec2(0.0f, HOP_SPEED * 0.7f); break;
                    case 1: m_hopVelocity = Vec2(0.0f, -HOP_SPEED * 0.7f); break;
                    case 2: m_hopVelocity = Vec2(-HOP_SPEED * 0.7f, 0.0f); break;
                    case 3: default: m_hopVelocity = Vec2(HOP_SPEED * 0.7f, 0.0f); break;
                }
            }

            m_isHopping = true;
            m_hopTimer = 0.35f;
        }
    }
}

} // namespace Core
