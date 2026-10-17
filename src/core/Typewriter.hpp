#pragma once

#include <string>
#include <string_view>
#include <algorithm>

namespace Core {

class Typewriter {
public:
    explicit Typewriter(std::string fullText = "", float charDelaySecs = 0.028f);

    void reset(std::string fullText);
    bool update(float dt);
    void skip();

    std::string getDisplayedText() const;
    const std::string& getFullText() const { return m_fullText; }
    bool isComplete() const { return m_completed; }

private:
    std::string m_fullText;
    float m_charDelaySecs = 0.028f;
    float m_elapsedSecs = 0.0f;
    size_t m_charsRevealed = 0;
    bool m_completed = false;
};

} // namespace Core
