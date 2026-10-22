#include "core/BossGolem.hpp"
#include "core/Dungeon.hpp"
#include <cmath>
#include <algorithm>

namespace Core {

BossGolem::BossGolem(float x, float y)
    : m_pos(x, y) {
    reset(x, y);
}

void BossGolem::reset(float x, float y) {
    m_pos = Vec2{x, y};
    m_phase = BossPhase::ArmoredStomp;
    m_hp = DEFAULT_MAX_HP;
    m_maxHp = DEFAULT_MAX_HP;
    m_speed = 32.0f;
    m_phaseTimer = 0.0f;
    m_hurtTimer = 0.0f;
    m_rockCooldown = 1.4f;
    m_explosionTimer = 0.0f;
    m_stunHitsTaken = 0;
    m_explosionSeed = 0;
    m_roarEvent = false;
    m_rockFiredEvent = false;
    m_explosionEvent = false;
    m_rocks.clear();
    m_explosions.clear();
}

void BossGolem::update(float dt, const Vec2& playerPos, const Dungeon& dungeon) {
    if (m_hurtTimer > 0.0f) {
        m_hurtTimer = std::max(0.0f, m_hurtTimer - dt);
    }

    // Update active rock projectiles
    for (auto& rock : m_rocks) {
        if (!rock.active) continue;
        rock.update(dt);
        if (dungeon.checkProjectileCollision(rock.getHitbox())) {
            rock.active = false;
        }
    }
    std::erase_if(m_rocks, [](const RockProjectile& r) { return !r.active; });

    // Update active explosion particles
    for (auto& exp : m_explosions) {
        if (!exp.active) continue;
        exp.timer += dt;
        exp.pos += exp.velocity * dt;
        exp.radius += 12.0f * dt;
        if (exp.timer >= exp.duration) {
            exp.active = false;
        }
    }
    std::erase_if(m_explosions, [](const ExplosionParticle& e) { return !e.active; });

    if (m_phase == BossPhase::Defeated) {
        return;
    }

    m_phaseTimer += dt;

    switch (m_phase) {
        case BossPhase::ArmoredStomp: {
            // Stomp toward the player
            Vec2 center = Vec2{m_pos.x + 16.0f, m_pos.y + 16.0f};
            Vec2 target = Vec2{playerPos.x + 8.0f, playerPos.y + 8.0f};
            Vec2 diff = target - center;
            float dist = diff.length();

            if (dist > 4.0f) {
                Vec2 dir = diff.normalized();
                Vec2 disp = dir * m_speed * dt;
                m_pos = dungeon.resolveMovement(m_pos, disp, m_localHitbox);
            }

            // Periodically hurl rocks at the player
            m_rockCooldown -= dt;
            if (m_rockCooldown <= 0.0f) {
                spawnRockAtPlayer(playerPos);
                m_rockCooldown = std::max(0.75f, 1.5f - (m_speed - 32.0f) * 0.015f);
            }

            // Transition to EyeCharging after stomp duration
            if (m_phaseTimer >= STOMP_PHASE_DURATION) {
                m_phase = BossPhase::EyeCharging;
                m_phaseTimer = 0.0f;
            }
            break;
        }

        case BossPhase::EyeCharging: {
            // Pauses movement while eye glows bright red to charge laser/shockwave
            if (m_phaseTimer >= CHARGE_PHASE_DURATION) {
                // Unleashes radial rock burst if player didn't stun it in time
                spawnRadialRockBurst();
                m_phase = BossPhase::ArmoredStomp;
                m_phaseTimer = 0.0f;
                m_rockCooldown = 1.2f;
            }
            break;
        }

        case BossPhase::Stunned: {
            // Immobilized for 3.5 seconds; recovers early if max combo hits reached
            if (m_phaseTimer >= STUN_DURATION || m_stunHitsTaken >= MAX_HITS_PER_STUN) {
                m_phase = BossPhase::ArmoredStomp;
                m_phaseTimer = 0.0f;
                m_stunHitsTaken = 0;
                m_speed += 10.0f; // Enrages and speeds up after each stun phase
                m_roarEvent = true;
                m_rockCooldown = 1.0f;
            }
            break;
        }

        case BossPhase::Dying: {
            m_rocks.clear();
            m_explosionTimer -= dt;
            if (m_explosionTimer <= 0.0f) {
                spawnExplosionWave();
                m_explosionTimer = 0.25f;
            }

            if (m_phaseTimer >= DYING_DURATION) {
                m_phase = BossPhase::Defeated;
                m_phaseTimer = 0.0f;
            }
            break;
        }

        case BossPhase::Defeated:
            break;
    }
}

bool BossGolem::tryArrowHit(const Rect& arrowHitbox, int damage) {
    if (m_phase == BossPhase::Dying || m_phase == BossPhase::Defeated) {
        return false;
    }

    if (!arrowHitbox.overlaps(getEyeHitbox()) && !arrowHitbox.overlaps(getBodyHitbox())) {
        return false;
    }

    // Only vulnerable to arrow stun during Phase 2: EyeCharging!
    if (m_phase == BossPhase::EyeCharging && arrowHitbox.overlaps(getEyeHitbox())) {
        m_hp = std::max(0, m_hp - std::max(1, damage));
        m_hurtTimer = 0.25f;

        if (m_hp <= 0) {
            m_phase = BossPhase::Dying;
            m_phaseTimer = 0.0f;
            m_roarEvent = true;
            spawnExplosionWave();
        } else {
            m_phase = BossPhase::Stunned;
            m_phaseTimer = 0.0f;
            m_stunHitsTaken = 0;
        }
        return true;
    }

    return false;
}

bool BossGolem::trySwordHit(const Rect& swordHitbox, int damage) {
    if (m_phase == BossPhase::Dying || m_phase == BossPhase::Defeated) {
        return false;
    }

    if (!swordHitbox.overlaps(getBodyHitbox())) {
        return false;
    }

    // Only vulnerable to sword strikes during Phase 3: Stunned!
    if (m_phase != BossPhase::Stunned) {
        return false;
    }

    if (m_hurtTimer > 0.0f) {
        return false;
    }

    m_hp = std::max(0, m_hp - std::max(1, damage));
    m_hurtTimer = 0.22f;
    m_stunHitsTaken++;

    if (m_hp <= 0) {
        m_phase = BossPhase::Dying;
        m_phaseTimer = 0.0f;
        m_roarEvent = true;
        spawnExplosionWave();
    }

    return true;
}

float BossGolem::getStunTimeRemaining() const {
    if (m_phase != BossPhase::Stunned) return 0.0f;
    return std::max(0.0f, STUN_DURATION - m_phaseTimer);
}

bool BossGolem::consumeRoarEvent() {
    bool v = m_roarEvent;
    m_roarEvent = false;
    return v;
}

bool BossGolem::consumeRockFiredEvent() {
    bool v = m_rockFiredEvent;
    m_rockFiredEvent = false;
    return v;
}

bool BossGolem::consumeExplosionEvent() {
    bool v = m_explosionEvent;
    m_explosionEvent = false;
    return v;
}

std::string BossGolem::getTextureName() const {
    switch (m_phase) {
        case BossPhase::ArmoredStomp:
            return "boss_golem_armored";
        case BossPhase::EyeCharging:
            return "boss_golem_charging";
        case BossPhase::Stunned:
        case BossPhase::Dying:
        case BossPhase::Defeated:
            return "boss_golem_stunned";
    }
    return "boss_golem_armored";
}

void BossGolem::spawnRockAtPlayer(const Vec2& playerPos) {
    Vec2 origin = Vec2{m_pos.x + 8.0f, m_pos.y + 8.0f};
    Vec2 dir = (playerPos - origin).normalized();
    if (dir.length() < 0.001f) {
        dir = Vec2{0.0f, 1.0f};
    }

    RockProjectile rock;
    rock.pos = origin;
    rock.velocity = dir * 115.0f;
    rock.active = true;
    rock.damage = 1;
    m_rocks.push_back(rock);
    m_rockFiredEvent = true;
}

void BossGolem::spawnRadialRockBurst() {
    Vec2 origin = Vec2{m_pos.x + 8.0f, m_pos.y + 8.0f};
    constexpr int NUM_DIRS = 8;
    for (int i = 0; i < NUM_DIRS; ++i) {
        float angle = static_cast<float>(i) * (2.0f * 3.14159265f / NUM_DIRS);
        Vec2 dir{std::cos(angle), std::sin(angle)};
        RockProjectile rock;
        rock.pos = origin;
        rock.velocity = dir * 125.0f;
        rock.active = true;
        rock.damage = 1;
        m_rocks.push_back(rock);
    }
    m_rockFiredEvent = true;
}

void BossGolem::spawnExplosionWave() {
    Vec2 center = Vec2{m_pos.x + 16.0f, m_pos.y + 16.0f};
    m_explosionSeed++;
    for (int i = 0; i < 6; ++i) {
        float angle = static_cast<float>(i) * (2.0f * 3.14159265f / 6.0f) + m_explosionSeed * 0.5f;
        ExplosionParticle p;
        p.pos = center + Vec2{std::cos(angle) * 6.0f, std::sin(angle) * 6.0f};
        p.velocity = Vec2{std::cos(angle) * 38.0f, std::sin(angle) * 38.0f};
        p.timer = 0.0f;
        p.duration = 0.45f;
        p.radius = 4.0f;
        p.active = true;
        m_explosions.push_back(p);
    }
    m_explosionEvent = true;
}

} // namespace Core
