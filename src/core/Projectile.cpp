#include "core/Projectile.hpp"

namespace Core {

Arrow::Arrow(const Vec2& spawnPos, Direction dir, float speed)
    : m_pos(spawnPos), m_direction(dir), m_speed(speed) {
    m_velocity = Vec2::fromDirection(dir) * speed;
}

void Arrow::update(float dt) {
    if (!m_active) return;

    m_pos += m_velocity * dt;
    m_lifetime -= dt;
    if (m_lifetime <= 0.0f) {
        m_active = false;
    }
}

Rect Arrow::getHitbox() const {
    switch (m_direction) {
        case Direction::Up:
        case Direction::Down:
            return Rect{m_pos.x + 6.0f, m_pos.y + 1.0f, 4.0f, 14.0f};
        case Direction::Left:
        case Direction::Right:
            return Rect{m_pos.x + 1.0f, m_pos.y + 6.0f, 14.0f, 4.0f};
    }
    return Rect{m_pos.x + 4.0f, m_pos.y + 4.0f, 8.0f, 8.0f};
}

std::string Arrow::getTextureName() const {
    switch (m_direction) {
        case Direction::Up:    return "arrow_up";
        case Direction::Down:  return "arrow_down";
        case Direction::Left:  return "arrow_left";
        case Direction::Right: return "arrow_right";
    }
    return "arrow_down";
}

} // namespace Core
