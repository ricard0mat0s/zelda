#pragma once

#include "core/Typewriter.hpp"
#include "raylib.h"
#include <string>
#include <functional>

namespace Render {

class DialogueBox {
public:
    static constexpr int BOX_X = 20;
    static constexpr int BOX_Y = 170;
    static constexpr int BOX_WIDTH = 280;
    static constexpr int BOX_HEIGHT = 58;

    DialogueBox();

    void show(std::string message, std::function<void()> onComplete = nullptr);
    bool advance(); // Returns true if dialogue finished and closed

    void update(float dt);
    void render();

    bool isOpen() const { return m_visible; }
    void close() { m_visible = false; }
    const std::string& getFullText() const { return m_typewriter.getFullText(); }
    std::string getDisplayedText() const { return m_typewriter.getDisplayedText(); }
    bool isTextComplete() const { return m_typewriter.isComplete(); }

private:
    Core::Typewriter m_typewriter;
    bool m_visible = false;
    float m_promptTimer = 0.0f;
    std::function<void()> m_onComplete;
};

} // namespace Render
