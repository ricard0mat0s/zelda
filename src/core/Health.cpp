#include "core/Health.hpp"

namespace Core {

Health::Health(int initialHearts) {
    m_maxHearts = std::max(1, initialHearts);
    m_currentHP = m_maxHearts * HP_PER_HEART;
    m_invulnerabilityTimer = 0.0f;
}

DamageResult Health::takeDamage(int amount) {
    if (isDead() || isInvulnerable() || amount <= 0) {
        return { false, isDead(), m_currentHP };
    }

    m_currentHP = std::max(0, m_currentHP - amount);
    m_invulnerabilityTimer = DEFAULT_INVULNERABILITY_SECS;

    return { true, isDead(), m_currentHP };
}

void Health::heal(int amount) {
    if (isDead() || amount <= 0) {
        return;
    }

    m_currentHP = std::min(getMaxHP(), m_currentHP + amount);
}

void Health::addHeartContainer() {
    m_maxHearts++;
    m_currentHP = getMaxHP();
}

void Health::update(float dt) {
    if (m_invulnerabilityTimer > 0.0f) {
        m_invulnerabilityTimer = std::max(0.0f, m_invulnerabilityTimer - dt);
    }
}

std::vector<HeartState> Health::getHeartStates() const {
    std::vector<HeartState> states;
    states.reserve(m_maxHearts);

    int hpLeft = m_currentHP;
    for (int i = 0; i < m_maxHearts; ++i) {
        if (hpLeft >= 2) {
            states.push_back(HeartState::Full);
            hpLeft -= 2;
        } else if (hpLeft == 1) {
            states.push_back(HeartState::Half);
            hpLeft -= 1;
        } else {
            states.push_back(HeartState::Empty);
        }
    }

    return states;
}

bool Health::isFlickerVisible() const {
    if (!isInvulnerable()) {
        return true;
    }
    // Flicker toggle every ~0.08 seconds
    int cycle = static_cast<int>(std::floor(m_invulnerabilityTimer / 0.08f));
    return (cycle % 2) == 0;
}

} // namespace Core
