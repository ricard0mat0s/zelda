#pragma once

#include "core/types.hpp"
#include "core/Movement.hpp"
#include "core/MapData.hpp"
#include "core/Health.hpp"
#include "core/Inventory.hpp"
#include "core/Combat.hpp"
#include <string>

namespace Core {

class Player {
public:
    Player(float x = 200.0f, float y = 280.0f);

    void update(float dt, const MovementInput& input, const MapData& map);

    Vec2 getPosition() const { return m_pos; }
    void setPosition(const Vec2& pos) { m_pos = pos; }

    Vec2 getVelocity() const { return m_velocity; }
    Direction getFacing() const { return m_facing; }
    bool isMoving() const { return m_isMoving; }

    Rect getLocalHitbox() const { return m_localHitbox; }
    Rect getWorldHitbox() const {
        return Rect(m_pos.x + m_localHitbox.x, m_pos.y + m_localHitbox.y,
                    m_localHitbox.width, m_localHitbox.height);
    }

    std::string getTextureName() const;

    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

    Health& getHealth() { return m_health; }
    const Health& getHealth() const { return m_health; }

    DamageResult takeDamage(int amount) { return m_health.takeDamage(amount); }
    void heal(int amount) { m_health.heal(amount); }

    Inventory& getInventory() { return m_inventory; }
    const Inventory& getInventory() const { return m_inventory; }

    bool isAttacking() const { return m_isAttacking; }
    bool startAttack();
    Rect getCurrentSwordHitbox() const;
    std::string getSlashTextureName() const;

private:
    Vec2 m_pos;
    Vec2 m_velocity{0.0f, 0.0f};
    Direction m_facing = Direction::Down;
    bool m_isMoving = false;
    float m_speed = 90.0f; // px/sec

    Health m_health{3};
    Inventory m_inventory;

    bool m_isAttacking = false;
    float m_attackTimer = 0.0f;
    float m_attackCooldown = 0.0f;

    Rect m_localHitbox{2.0f, 6.0f, 12.0f, 10.0f};

    int m_animFrame = 0;
    float m_animTimer = 0.0f;
    static constexpr float FRAME_DURATION = 0.15f;
};

} // namespace Core
