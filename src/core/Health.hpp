#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

namespace Core {

enum class HeartState {
    Full,
    Half,
    Empty
};

struct DamageResult {
    bool damaged = false;
    bool isDead = false;
    int remainingHP = 0;
};

class Health {
public:
    static constexpr int HP_PER_HEART = 2;
    static constexpr float DEFAULT_INVULNERABILITY_SECS = 1.0f;

    explicit Health(int initialHearts = 3);

    int getMaxHearts() const { return m_maxHearts; }
    int getMaxHP() const { return m_maxHearts * HP_PER_HEART; }
    int getCurrentHP() const { return m_currentHP; }

    bool isDead() const { return m_currentHP <= 0; }
    bool isInvulnerable() const { return m_invulnerabilityTimer > 0.0f; }
    float getInvulnerabilityTimer() const { return m_invulnerabilityTimer; }

    DamageResult takeDamage(int amount);
    void heal(int amount);
    void addHeartContainer();

    void update(float dt);

    // Computes heart display states for UI HUD
    std::vector<HeartState> getHeartStates() const;

    // Returns false during invulnerability flicker off-frames
    bool isFlickerVisible() const;

private:
    int m_maxHearts = 3;
    int m_currentHP = 6;
    float m_invulnerabilityTimer = 0.0f;
};

} // namespace Core
