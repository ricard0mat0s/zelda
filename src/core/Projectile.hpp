#pragma once

#include "core/types.hpp"
#include <string>

namespace Core {

class Arrow {
public:
    Arrow(const Vec2& spawnPos, Direction dir, float speed = 240.0f);

    void update(float dt);

    Vec2 getPosition() const { return m_pos; }
    Direction getDirection() const { return m_direction; }
    bool isActive() const { return m_active; }
    void deactivate() { m_active = false; }

    Rect getHitbox() const;
    std::string getTextureName() const;

    int getDamage() const { return m_damage; }

private:
    Vec2 m_pos;
    Vec2 m_velocity;
    Direction m_direction;
    float m_speed;
    float m_lifetime = 1.8f;
    bool m_active = true;
    int m_damage = 1;
};

} // namespace Core
