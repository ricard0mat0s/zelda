#pragma once

#include "core/types.hpp"
#include <string>
#include <vector>

namespace Core {

class Dungeon;

enum class BossPhase {
    ArmoredStomp = 0, // Phase 1: Stomps toward player & hurls rocks; impervious to sword
    EyeCharging,      // Phase 2: Pauses with glowing red eye; arrow hit stuns for 3.5s
    Stunned,          // Phase 3: Vulnerable to sword combo; roars & speeds up after recovery
    Dying,            // Death sequence with screen shake & explosion particles
    Defeated          // Fully defeated
};

struct RockProjectile {
    Vec2 pos;
    Vec2 velocity;
    bool active = true;
    int damage = 1;
    float maxDistance = 300.0f;
    float traveled = 0.0f;

    Rect getHitbox() const {
        return Rect{pos.x + 4.0f, pos.y + 4.0f, 8.0f, 8.0f};
    }

    void update(float dt) {
        if (!active) return;
        Vec2 step = velocity * dt;
        pos += step;
        traveled += step.length();
        if (traveled >= maxDistance) {
            active = false;
        }
    }
};

struct ExplosionParticle {
    Vec2 pos;
    Vec2 velocity;
    float timer = 0.0f;
    float duration = 0.45f;
    float radius = 4.0f;
    bool active = true;
};

class BossGolem {
public:
    static constexpr int DEFAULT_MAX_HP = 12;
    static constexpr float STOMP_PHASE_DURATION = 4.0f;
    static constexpr float CHARGE_PHASE_DURATION = 2.5f;
    static constexpr float STUN_DURATION = 3.5f;
    static constexpr float DYING_DURATION = 2.0f;
    static constexpr int MAX_HITS_PER_STUN = 4;

    explicit BossGolem(float x = 144.0f, float y = 56.0f);

    void reset(float x = 144.0f, float y = 56.0f);
    void update(float dt, const Vec2& playerPos, const Dungeon& dungeon);

    // Returns true if arrow hits the vulnerable charging eye and stuns the Golem
    bool tryArrowHit(const Rect& arrowHitbox, int damage = 1);

    // Returns true if sword deals damage (only during Stunned phase); false if deflected by armor
    bool trySwordHit(const Rect& swordHitbox, int damage = 1);

    BossPhase getPhase() const { return m_phase; }
    void setPhase(BossPhase phase) { m_phase = phase; m_phaseTimer = 0.0f; }

    Vec2 getPosition() const { return m_pos; }
    void setPosition(const Vec2& pos) { m_pos = pos; }

    int getHP() const { return m_hp; }
    int getMaxHP() const { return m_maxHp; }
    float getSpeed() const { return m_speed; }
    float getPhaseTimer() const { return m_phaseTimer; }
    float getStunTimeRemaining() const;

    bool isDead() const { return m_hp <= 0; }
    bool isDying() const { return m_phase == BossPhase::Dying; }
    bool isDefeated() const { return m_phase == BossPhase::Defeated; }
    bool isHurt() const { return m_hurtTimer > 0.0f; }
    bool isArmored() const { return m_phase == BossPhase::ArmoredStomp || m_phase == BossPhase::EyeCharging; }

    // Event flags consumed by scene for audio/visual feedback
    bool consumeRoarEvent();
    bool consumeRockFiredEvent();
    bool consumeExplosionEvent();

    // 32x32 world footprint (16x16 sprite rendered at 2x scale)
    Rect getLocalHitbox() const { return m_localHitbox; }
    Rect getBodyHitbox() const {
        return Rect{m_pos.x + m_localHitbox.x, m_pos.y + m_localHitbox.y,
                    m_localHitbox.width, m_localHitbox.height};
    }
    Rect getEyeHitbox() const {
        return Rect{m_pos.x + 6.0f, m_pos.y + 6.0f, 20.0f, 16.0f};
    }

    std::string getTextureName() const;

    std::vector<RockProjectile>& getRocks() { return m_rocks; }
    const std::vector<RockProjectile>& getRocks() const { return m_rocks; }

    const std::vector<ExplosionParticle>& getExplosions() const { return m_explosions; }

private:
    void spawnRockAtPlayer(const Vec2& playerPos);
    void spawnRadialRockBurst();
    void spawnExplosionWave();

    Vec2 m_pos;
    BossPhase m_phase = BossPhase::ArmoredStomp;
    int m_hp = DEFAULT_MAX_HP;
    int m_maxHp = DEFAULT_MAX_HP;

    float m_speed = 32.0f;
    float m_phaseTimer = 0.0f;
    float m_hurtTimer = 0.0f;
    float m_rockCooldown = 1.4f;
    float m_explosionTimer = 0.0f;
    int m_stunHitsTaken = 0;
    int m_explosionSeed = 0;

    bool m_roarEvent = false;
    bool m_rockFiredEvent = false;
    bool m_explosionEvent = false;

    Rect m_localHitbox{4.0f, 4.0f, 24.0f, 24.0f};
    std::vector<RockProjectile> m_rocks;
    std::vector<ExplosionParticle> m_explosions;
};

using BossGolemStateMachine = BossGolem;

} // namespace Core
