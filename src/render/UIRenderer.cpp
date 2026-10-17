#include "render/UIRenderer.hpp"
#include "render/AssetManager.hpp"
#include "core/GameConfig.hpp"

namespace Render {

void UIRenderer::renderHearts(const Core::Health& health) {
    auto& assets = AssetManager::instance();
    Texture2D fullHeart = assets.getTexture("heart_full");
    Texture2D halfHeart = assets.getTexture("heart_half");
    Texture2D emptyHeart = assets.getTexture("heart_empty");

    // 1. Header: "- LIFE -"
    const char* header = "- LIFE -";
    constexpr int startX = 14;
    constexpr int headerY = 8;
    DrawText(header, startX + 1, headerY + 1, 8, Color{11, 12, 16, 255});
    DrawText(header, startX, headerY, 8, Color{229, 62, 62, 255});

    // 2. Heart Containers
    constexpr int heartsY = 18;
    constexpr int heartSpacing = 14;

    const auto states = health.getHeartStates();
    for (size_t i = 0; i < states.size(); ++i) {
        int hx = startX + static_cast<int>(i) * heartSpacing;
        Texture2D tex;
        switch (states[i]) {
            case Core::HeartState::Full:
                tex = fullHeart;
                break;
            case Core::HeartState::Half:
                tex = halfHeart;
                break;
            case Core::HeartState::Empty:
            default:
                tex = emptyHeart;
                break;
        }
        DrawTexture(tex, hx, heartsY, WHITE);
    }
}

void UIRenderer::renderGameOver() {
    const char* overText = "GAME OVER";
    int textW = MeasureText(overText, 16);
    int cx = (Core::GameConfig::VIRTUAL_WIDTH - textW) / 2;
    int cy = Core::GameConfig::VIRTUAL_HEIGHT / 2 - 20;

    DrawRectangle(0, 0, Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT, Color{0, 0, 0, 180});
    DrawText(overText, cx + 1, cy + 1, 16, Color{11, 12, 16, 255});
    DrawText(overText, cx, cy, 16, Color{229, 62, 62, 255});

    const char* retryText = "PRESS ENTER TO RETRY";
    int rW = MeasureText(retryText, 8);
    int rx = (Core::GameConfig::VIRTUAL_WIDTH - rW) / 2;
    DrawText(retryText, rx, cy + 28, 8, WHITE);
}

} // namespace Render
