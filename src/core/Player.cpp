#include "core/Player.hpp"
#include "core/GameConfig.hpp"
#include "core/Dungeon.hpp"

namespace Core {

Player::Player(float x, float y)
    : m_pos(x, y) {}

bool Player::startAttack() {
    if (canPerformAttack(m_inventory.hasSword, m_isAttacking, m_attackCooldown)) {
        m_isAttacking = true;
        m_attackTimer = GameConfig::SWORD_ATTACK_DURATION_SECS;
        m_attackCooldown = GameConfig::SWORD_ATTACK_DURATION_SECS + 0.1f;
        m_velocity = {0.0f, 0.0f};
        m_isMoving = false;
        m_animFrame = 0;
        return true;
    }
    return false;
}

Rect Player::getCurrentSwordHitbox() const {
    if (!m_isAttacking) {
        return Rect(0.0f, 0.0f, 0.0f, 0.0f);
    }
    return getSwordHitbox(m_pos, m_facing);
}

std::string Player::getSlashTextureName() const {
    switch (m_facing) {
        case Direction::Down:  return "slash_down";
        case Direction::Up:    return "slash_up";
        case Direction::Left:  return "slash_left";
        case Direction::Right: return "slash_right";
    }
    return "slash_down";
}

void Player::update(float dt, const MovementInput& input, const MapData& map) {
    m_health.update(dt);

    if (m_attackCooldown > 0.0f) {
        m_attackCooldown -= dt;
    }

    if (m_isAttacking) {
        m_attackTimer -= dt;
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
        }
    }

    if (m_health.isDead() || m_isAttacking) {
        m_velocity = {0.0f, 0.0f};
        m_isMoving = false;
        m_animFrame = 0;
        return;
    }

    MovementResult moveRes = computeMovementVelocity(input, m_facing, m_speed);
    m_velocity = moveRes.velocity;
    m_facing = moveRes.facing;
    m_isMoving = moveRes.isMoving;

    if (m_isMoving) {
        Vec2 displacement = m_velocity * dt;
        m_pos = map.resolveMovement(m_pos, displacement, m_localHitbox);

        m_animTimer += dt;
        if (m_animTimer >= FRAME_DURATION) {
            m_animTimer -= FRAME_DURATION;
            m_animFrame = (m_animFrame + 1) % 2;
        }
    } else {
        m_animFrame = 0;
        m_animTimer = 0.0f;
    }
}

void Player::updateInDungeon(float dt, const MovementInput& input, const Dungeon& dungeon) {
    m_health.update(dt);

    if (m_attackCooldown > 0.0f) {
        m_attackCooldown -= dt;
    }

    if (m_isAttacking) {
        m_attackTimer -= dt;
        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
        }
    }

    if (m_health.isDead() || m_isAttacking) {
        m_velocity = {0.0f, 0.0f};
        m_isMoving = false;
        m_animFrame = 0;
        return;
    }

    MovementResult moveRes = computeMovementVelocity(input, m_facing, m_speed);
    m_velocity = moveRes.velocity;
    m_facing = moveRes.facing;
    m_isMoving = moveRes.isMoving;

    if (m_isMoving) {
        Vec2 displacement = m_velocity * dt;
        m_pos = dungeon.resolveMovement(m_pos, displacement, m_localHitbox);

        m_animTimer += dt;
        if (m_animTimer >= FRAME_DURATION) {
            m_animTimer -= FRAME_DURATION;
            m_animFrame = (m_animFrame + 1) % 2;
        }
    } else {
        m_animFrame = 0;
        m_animTimer = 0.0f;
    }
}

std::string Player::getTextureName() const {
    std::string base = "hero_";
    switch (m_facing) {
        case Direction::Down:  base += "down_"; break;
        case Direction::Up:    base += "up_"; break;
        case Direction::Left:  base += "left_"; break;
        case Direction::Right: base += "right_"; break;
    }
    base += std::to_string(m_animFrame);
    return base;
}

} // namespace Core
