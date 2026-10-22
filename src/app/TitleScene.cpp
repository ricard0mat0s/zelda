#include "app/TitleScene.hpp"
#include "raylib.h"
#include "core/GameConfig.hpp"
#include "core/SaveSystem.hpp"
#include "render/AudioPlayer.hpp"
#include <cmath>

namespace App {

TitleScene::TitleScene() {
    reset();
}

void TitleScene::reset() {
    m_timer = 0.0f;
    m_promptVisible = true;
    m_startRequested = false;
    m_continueRequested = false;
    m_hasSave = Core::SaveSystem::hasSaveFile();
    m_fadeProgress = 0.0f;
}

void TitleScene::update(float dt) {
    m_timer += dt;

    if (!m_startRequested) {
        m_hasSave = Core::SaveSystem::hasSaveFile();

        // Blinking prompt logic: visible for 1.2s, invisible for 0.2s
        float cycle = std::fmod(m_timer, 1.4f);
        m_promptVisible = (cycle < 1.2f);

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            m_startRequested = true;
            m_continueRequested = m_hasSave;
            Render::AudioPlayer::instance().playGameStart();
        } else if (m_hasSave && IsKeyPressed(KEY_N)) {
            Core::SaveSystem::deleteSaveFile();
            m_hasSave = false;
            m_startRequested = true;
            m_continueRequested = false;
            Render::AudioPlayer::instance().playGameStart();
        }
    } else {
        m_fadeProgress += dt * 3.0f; // Quick fade out over ~0.33s
    }
}

static void drawCenteredText(const char* text, int y, int fontSize, Color color, Color shadowColor, int shadowOffset = 1) {
    int textWidth = MeasureText(text, fontSize);
    int x = (Core::GameConfig::VIRTUAL_WIDTH - textWidth) / 2;
    if (shadowOffset > 0) {
        DrawText(text, x + shadowOffset, y + shadowOffset, fontSize, shadowColor);
    }
    DrawText(text, x, y, fontSize, color);
}

void TitleScene::render() {
    const int cx = Core::GameConfig::VIRTUAL_WIDTH / 2;

    // 1. Background (Dark retro slate: #0a0c10)
    DrawRectangle(0, 0, Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT, Color{10, 12, 16, 255});

    // 2. Decorative Golden Triforce
    const float triSize = 16.0f;
    const float triH = triSize * 1.5f;
    const float triY = 32.0f;
    const Color goldColor = Color{255, 215, 0, 255};
    const Color goldShadow = Color{116, 66, 16, 255};

    auto drawOneTriangle = [&](float topX, float topY) {
        Vector2 vTop = { topX, topY };
        Vector2 vBottomLeft = { topX - triSize, topY + triH };
        Vector2 vBottomRight = { topX + triSize, topY + triH };

        // Shadow triangle
        DrawTriangle(
            Vector2{vBottomLeft.x + 1, vBottomLeft.y + 1},
            Vector2{vBottomRight.x + 1, vBottomRight.y + 1},
            Vector2{vTop.x + 1, vTop.y + 1},
            goldShadow
        );
        // Main triangle (counter-clockwise)
        DrawTriangle(vBottomLeft, vBottomRight, vTop, goldColor);
    };

    // Top triangle
    drawOneTriangle(cx, triY);
    // Bottom-left triangle
    drawOneTriangle(cx - triSize, triY + triH);
    // Bottom-right triangle
    drawOneTriangle(cx + triSize, triY + triH);

    // 3. Title typography
    drawCenteredText("THE LEGEND OF", 96, 10, Color{160, 174, 192, 255}, Color{15, 20, 25, 255});
    drawCenteredText("ARCADE HERO", 112, 20, goldColor, goldShadow, 2);
    drawCenteredText("16-BIT RETRO ADVENTURE", 140, 10, Color{144, 205, 244, 255}, Color{10, 30, 60, 255});

    // 4. Prompt: "PRESS SPACE TO START" or "CONTINUE / NEW GAME"
    if (m_promptVisible) {
        if (m_hasSave) {
            drawCenteredText("PRESS ENTER TO CONTINUE", 170, 10, Color{250, 204, 21, 255}, BLACK);
            drawCenteredText("PRESS [N] FOR NEW GAME", 186, 8, WHITE, BLACK);
        } else {
            drawCenteredText("PRESS SPACE TO START", 175, 10, WHITE, BLACK);
        }
    }

    // 5. Controls hint
    drawCenteredText("WASD/ARROWS: MOVE | SPACE: ATTACK", 215, 8, Color{200, 200, 200, 255}, BLACK);

    // 6. Fade transition
    if (m_startRequested && m_fadeProgress > 0.0f) {
        unsigned char alpha = static_cast<unsigned char>(std::min(1.0f, m_fadeProgress) * 255.0f);
        DrawRectangle(0, 0, Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT, Color{0, 0, 0, alpha});
    }
}

} // namespace App
