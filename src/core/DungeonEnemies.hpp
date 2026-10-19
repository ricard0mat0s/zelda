#pragma once

#include "core/types.hpp"
#include <string>

namespace Core {

class Dungeon;

class Bat {
public:
    explicit Bat(float x = 0.0f, float y = 0.0f);

    void update(float dt, const Vec2& playerPos);
    bool takeDamage(int amount = 1);

    Vec2 getPosition() const { return m_pos; }
    void setPosition(const Vec2& pos) { m_pos = pos; }

    int getHP() const { return m_hp; }
    bool isDead() const { return m_isDead; }
    bool isHurt() const { return m_hurtTimer > 0.0f; }

    Rect getHitbox() const {
        return Rect{m_pos.x + 2.0f, m_pos.y + 3.0f, 12.0f, 10.0f};
    }

    std::string getTextureName() const;

private:
    Vec2 m_pos;
    Vec2 m_basePos;
    int m_hp = 1;
    bool m_isDead = false;

    float m_stateTimer = 0.0f;
    float m_animTimer = 0.0f;
    int m_animFrame = 0;
    float m_hurtTimer = 0.0f;
    bool m_swooping = false;
};

class Skeleton {
public:
    explicit Skeleton(float x = 0.0f, float y = 0.0f);

    void update(float dt, const Vec2& playerPos, const Dungeon& dungeon);
    bool takeDamage(int amount, const Vec2& knockbackVelocity);

    Vec2 getPosition() const { return m_pos; }
    void setPosition(const Vec2& pos) { m_pos = pos; }

    int getHP() const { return m_hp; }
    bool isDead() const { return m_isDead; }
    bool isHurt() const { return m_hurtTimer > 0.0f; }
    Direction getFacing() const { return m_facing; }

    Rect getLocalHitbox() const { return m_localHitbox; }
    Rect getHitbox() const {
        return Rect{m_pos.x + m_localHitbox.x, m_pos.y + m_localHitbox.y,
                    m_localHitbox.width, m_localHitbox.height};
    }

    std::string getTextureName() const;

private:
    Vec2 m_pos;
    Vec2 m_velocity{0.0f, 0.0f};
    Vec2 m_knockback{0.0f, 0.0f};
    Direction m_facing = Direction::Down;

    int m_hp = 3;
    bool m_isDead = false;
    float m_hurtTimer = 0.0f;

    float m_animTimer = 0.0f;
    int m_animFrame = 0;
    float m_patrolTimer = 0.0f;

    Rect m_localHitbox{2.0f, 2.0f, 12.0f, 14.0f};
    float m_speed = 45.0f;
};

} // namespace Core
