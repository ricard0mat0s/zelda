#include "render/DialogueBox.hpp"
#include <cmath>

namespace Render {

DialogueBox::DialogueBox() = default;

void DialogueBox::show(std::string message, std::function<void()> onComplete) {
    m_typewriter.reset(std::move(message));
    m_onComplete = std::move(onComplete);
    m_promptTimer = 0.0f;
    m_visible = true;
}

bool DialogueBox::advance() {
    if (!m_visible) {
        return false;
    }

    if (!m_typewriter.isComplete()) {
        m_typewriter.skip();
        return false;
    }

    m_visible = false;
    if (m_onComplete) {
        auto cb = std::move(m_onComplete);
        m_onComplete = nullptr;
        cb();
    }
    return true;
}

void DialogueBox::update(float dt) {
    if (!m_visible) return;

    if (!m_typewriter.isComplete()) {
        m_typewriter.update(dt);
    } else {
        m_promptTimer += dt;
    }
}

void DialogueBox::render() {
    if (!m_visible) return;

    // 1. Background box
    DrawRectangle(BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT, Color{11, 12, 16, 240});

    // 2. Outer golden border (2px)
    DrawRectangleLinesEx(Rectangle{static_cast<float>(BOX_X), static_cast<float>(BOX_Y),
                                   static_cast<float>(BOX_WIDTH), static_cast<float>(BOX_HEIGHT)},
                         2.0f, Color{255, 215, 0, 255});

    // 3. Inner slate border (1px)
    DrawRectangleLinesEx(Rectangle{static_cast<float>(BOX_X + 3), static_cast<float>(BOX_Y + 3),
                                   static_cast<float>(BOX_WIDTH - 6), static_cast<float>(BOX_HEIGHT - 6)},
                         1.0f, Color{113, 128, 150, 180});

    // 4. Message text
    std::string text = m_typewriter.getDisplayedText();
    DrawText(text.c_str(), BOX_X + 12 + 1, BOX_Y + 12 + 1, 8, Color{11, 12, 16, 255});
    DrawText(text.c_str(), BOX_X + 12, BOX_Y + 12, 8, WHITE);

    // 5. Blinking continue prompt: "▼ PRESS E"
    if (m_typewriter.isComplete()) {
        int blinkCycle = static_cast<int>(std::floor(m_promptTimer / 0.3f));
        if (blinkCycle % 2 == 0) {
            const char* prompt = "v PRESS E";
            int promptW = MeasureText(prompt, 7);
            int px = BOX_X + BOX_WIDTH - promptW - 12;
            int py = BOX_Y + BOX_HEIGHT - 14;

            DrawText(prompt, px + 1, py + 1, 7, Color{11, 12, 16, 255});
            DrawText(prompt, px, py, 7, Color{255, 215, 0, 255});
        }
    }
}

} // namespace Render
