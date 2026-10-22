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

void UIRenderer::renderInventory(const Core::Inventory& inventory) {
    auto& assets = AssetManager::instance();

    // Top-right item slots
    int rightX = Core::GameConfig::VIRTUAL_WIDTH - 24;
    int slotY = 8;

    // Slot [X]: Bow (if obtained)
    if (inventory.hasBow) {
        Texture2D bowTex = assets.getTexture("bow_icon");
        DrawRectangle(rightX - 2, slotY - 2, 20, 20, Color{11, 12, 16, 200});
        DrawRectangleLines(rightX - 2, slotY - 2, 20, 20, Color{212, 163, 115, 255});
        DrawTexture(bowTex, rightX, slotY, WHITE);

        // "X" badge
        DrawText("X", rightX - 8, slotY + 4, 8, Color{250, 204, 21, 255});
    }

    // Keys counter (if any)
    if (inventory.keys > 0) {
        int keyX = rightX - (inventory.hasBow ? 36 : 0);
        std::string keyStr = "K:" + std::to_string(inventory.keys);
        DrawText(keyStr.c_str(), keyX + 1, slotY + 5, 8, Color{11, 12, 16, 255});
        DrawText(keyStr.c_str(), keyX, slotY + 4, 8, Color{250, 204, 21, 255});
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

void UIRenderer::renderBossHealthBar(const char* name, int currentHP, int maxHP) {
    if (maxHP <= 0 || currentHP <= 0) return;

    constexpr int barWidth = 104;
    constexpr int barHeight = 6;
    int barX = (Core::GameConfig::VIRTUAL_WIDTH - barWidth) / 2;
    int barY = 18;

    int nameW = MeasureText(name, 8);
    int nameX = (Core::GameConfig::VIRTUAL_WIDTH - nameW) / 2;
    DrawText(name, nameX + 1, 8 + 1, 8, Color{11, 12, 16, 255});
    DrawText(name, nameX, 8, 8, Color{250, 204, 21, 255});

    // Bar frame & background
    DrawRectangle(barX - 2, barY - 2, barWidth + 4, barHeight + 4, Color{11, 12, 16, 230});
    DrawRectangleLines(barX - 1, barY - 1, barWidth + 2, barHeight + 2, Color{148, 163, 184, 255});

    float ratio = static_cast<float>(currentHP) / static_cast<float>(maxHP);
    int fillW = static_cast<int>(barWidth * std::clamp(ratio, 0.0f, 1.0f));
    if (fillW > 0) {
        DrawRectangle(barX, barY, fillW, barHeight, Color{220, 38, 38, 255});
        DrawRectangle(barX, barY, fillW, 2, Color{248, 113, 113, 255});
    }
}

void UIRenderer::renderVictoryScreen(int maxHearts) {
    DrawRectangle(0, 0, Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT, Color{11, 12, 16, 215});

    // Decorative gold border
    DrawRectangleLines(16, 16, Core::GameConfig::VIRTUAL_WIDTH - 32, Core::GameConfig::VIRTUAL_HEIGHT - 32, Color{250, 204, 21, 255});
    DrawRectangleLines(18, 18, Core::GameConfig::VIRTUAL_WIDTH - 36, Core::GameConfig::VIRTUAL_HEIGHT - 36, Color{202, 138, 4, 255});

    const char* title = "VICTORY!";
    int titleW = MeasureText(title, 20);
    int tx = (Core::GameConfig::VIRTUAL_WIDTH - titleW) / 2;
    DrawText(title, tx + 1, 43, 20, BLACK);
    DrawText(title, tx, 42, 20, Color{250, 204, 21, 255});

    const char* sub1 = "ANCIENT EYE GOLEM DEFEATED";
    int s1W = MeasureText(sub1, 8);
    DrawText(sub1, (Core::GameConfig::VIRTUAL_WIDTH - s1W) / 2, 74, 8, Color{103, 232, 249, 255});

    const char* sub2 = "PEACE RETURNS TO HYRULE";
    int s2W = MeasureText(sub2, 8);
    DrawText(sub2, (Core::GameConfig::VIRTUAL_WIDTH - s2W) / 2, 90, 8, Color{226, 232, 240, 255});

    // Render Heart Container icon + stat
    auto& assets = AssetManager::instance();
    Texture2D hcTex = assets.getTexture("heart_container");
    int iconX = Core::GameConfig::VIRTUAL_WIDTH / 2 - 8;
    DrawTexture(hcTex, iconX, 112, WHITE);

    std::string statStr = "MAX LIFE INCREASED TO " + std::to_string(maxHearts) + " HEARTS!";
    int stW = MeasureText(statStr.c_str(), 8);
    DrawText(statStr.c_str(), (Core::GameConfig::VIRTUAL_WIDTH - stW) / 2, 136, 8, Color{248, 113, 113, 255});

    const char* prompt = "PRESS [ENTER] TO CONTINUE ADVENTURE";
    int pW = MeasureText(prompt, 8);
    DrawText(prompt, (Core::GameConfig::VIRTUAL_WIDTH - pW) / 2, 184, 8, WHITE);
}

} // namespace Render
