#include "core/Typewriter.hpp"
#include <cmath>

namespace Core {

Typewriter::Typewriter(std::string fullText, float charDelaySecs)
    : m_fullText(std::move(fullText)),
      m_charDelaySecs(std::max(0.001f, charDelaySecs)) {
    reset(m_fullText);
}

void Typewriter::reset(std::string fullText) {
    m_fullText = std::move(fullText);
    m_elapsedSecs = 0.0f;
    m_charsRevealed = 0;
    m_completed = m_fullText.empty();
}

bool Typewriter::update(float dt) {
    if (m_completed) {
        return false;
    }

    m_elapsedSecs += dt;
    size_t newCount = std::min(
        m_fullText.size(),
        static_cast<size_t>(std::floor(m_elapsedSecs / m_charDelaySecs))
    );

    bool changed = (newCount != m_charsRevealed);
    m_charsRevealed = newCount;

    if (m_charsRevealed >= m_fullText.size()) {
        m_completed = true;
    }

    return changed;
}

void Typewriter::skip() {
    m_charsRevealed = m_fullText.size();
    m_completed = true;
}

std::string Typewriter::getDisplayedText() const {
    return m_fullText.substr(0, m_charsRevealed);
}

} // namespace Core
