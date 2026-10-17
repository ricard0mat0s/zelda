#pragma once

#include "core/types.hpp"
#include "core/MapData.hpp"

namespace Core {

class Slime {
public:
    explicit Slime(float x = 0.0f, float y = 0.0f);

    void update(float dt, const Vec2& playerPos, const MapData& map);

    bool takeDamage(int amount, const Vec2& knockbackVelocity);

    Vec2 getPosition() const { return m_pos; }
    void setPosition(const Vec2& pos) { m_pos = pos; }

    int getHP() const { return m_hp; }
    bool isDead() const { return m_isDead; }
    bool isHurt() const { return m_hurtTimer > 0.0f; }
    int getAnimFrame() const { return m_animFrame; }

    Rect getLocalHitbox() const { return m_localHitbox; }
    Rect getHitbox() const {
        return Rect(m_pos.x + m_localHitbox.x, m_pos.y + m_localHitbox.y,
                    m_localHitbox.width, m_localHitbox.height);
    }

private:
    Vec2 m_pos;
    Vec2 m_hopVelocity{0.0f, 0.0f};
    Vec2 m_knockback{0.0f, 0.0f};

    int m_hp = 2;
    bool m_isDead = false;

    float m_stateTimer = 0.0f;
    float m_hopTimer = 0.0f;
    float m_hopCooldown = 1.0f;
    float m_hurtTimer = 0.0f;

    bool m_isHopping = false;
    int m_animFrame = 0;

    Rect m_localHitbox{2.0f, 4.0f, 12.0f, 10.0f};
};

} // namespace Core
