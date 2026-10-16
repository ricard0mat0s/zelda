#include "core/Player.hpp"

namespace Core {

Player::Player(float x, float y)
    : m_pos(x, y) {}

void Player::update(float dt, const MovementInput& input, const MapData& map) {
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
