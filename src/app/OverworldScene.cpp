#include "app/OverworldScene.hpp"
#include "render/AssetManager.hpp"
#include "render/AudioPlayer.hpp"
#include "core/GameConfig.hpp"
#include <algorithm>
#include <cmath>

namespace App {

OverworldScene::OverworldScene() {
    init();
}

void OverworldScene::init() {
    reset();
}

void OverworldScene::reset() {
    m_player = Core::Player(200.0f, 280.0f);
    m_bannerTimer = 0.0f;

    m_camera.offset = Vector2{0.0f, 0.0f};
    m_camera.target = Vector2{0.0f, 0.0f};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

void OverworldScene::update(float dt) {
    // 0. If dead, allow retry
    if (m_player.getHealth().isDead()) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            reset();
        }
        return;
    }

    // Debug testing controls for health verification
    if (IsKeyPressed(KEY_H)) {
        auto res = m_player.takeDamage(1);
        if (res.damaged) {
            Render::AudioPlayer::instance().playPlayerHurt();
        }
    }
    if (IsKeyPressed(KEY_J)) {
        m_player.heal(1);
    }

    // 1. Process movement input
    Core::MovementInput input;
    input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    m_player.update(dt, input, m_map);

    // 2. Camera tracking clamped to world bounds
    const float halfVW = Core::GameConfig::VIRTUAL_WIDTH * 0.5f;
    const float halfVH = Core::GameConfig::VIRTUAL_HEIGHT * 0.5f;
    const float maxX = static_cast<float>(Core::GameConfig::WORLD_WIDTH - Core::GameConfig::VIRTUAL_WIDTH);
    const float maxY = static_cast<float>(Core::GameConfig::WORLD_HEIGHT - Core::GameConfig::VIRTUAL_HEIGHT);

    float targetX = std::clamp(m_player.getPosition().x + 8.0f - halfVW, 0.0f, maxX);
    float targetY = std::clamp(m_player.getPosition().y + 8.0f - halfVH, 0.0f, maxY);

    // Smooth camera tracking
    m_camera.target.x += (targetX - m_camera.target.x) * std::min(1.0f, dt * 10.0f);
    m_camera.target.y += (targetY - m_camera.target.y) * std::min(1.0f, dt * 10.0f);

    // 3. Banner timer
    m_bannerTimer += dt;
}

void OverworldScene::render() {
    auto& assets = Render::AssetManager::instance();

    // 1. World pass inside camera
    BeginMode2D(m_camera);

    // Render ground and obstacle tilemaps
    m_mapRenderer.render(m_map, m_camera);

    // Render Elder Shrine Pedestal holding the Hero's Sword
    Texture2D shrineTex = assets.getTexture("shrine_pedestal_sword");
    DrawTexture(shrineTex, static_cast<int>(m_shrinePos.x - 8.0f), static_cast<int>(m_shrinePos.y - 8.0f), WHITE);

    // Render Hero sprite with invulnerability flicker
    const auto& health = m_player.getHealth();
    Texture2D heroTex = assets.getTexture(m_player.getTextureName());
    int px = static_cast<int>(m_player.getPosition().x);
    int py = static_cast<int>(m_player.getPosition().y);

    if (health.isDead()) {
        DrawTexture(heroTex, px, py, Color{255, 100, 100, 220});
    } else if (health.isInvulnerable()) {
        if (health.isFlickerVisible()) {
            DrawTexture(heroTex, px, py, Color{255, 255, 255, 140});
        }
    } else {
        DrawTexture(heroTex, px, py, WHITE);
    }

    EndMode2D();

    // 2. Screen-space HUD & Overlays
    m_uiRenderer.renderHearts(m_player.getHealth());

    if (health.isDead()) {
        m_uiRenderer.renderGameOver();
    }

    // 2. Screen-space HUD & Location Banner
    if (m_bannerTimer < BANNER_DURATION) {
        float alpha = 1.0f;
        if (m_bannerTimer > BANNER_DURATION - 1.0f) {
            alpha = (BANNER_DURATION - m_bannerTimer); // Fade out over 1.0s
        }
        unsigned char a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);

        const char* title = "HYLIA GLADE";
        int textW = MeasureText(title, 10);
        int bx = (Core::GameConfig::VIRTUAL_WIDTH - textW) / 2;
        int by = 20;

        // Shadow and text
        DrawText(title, bx + 1, by + 1, 10, Color{11, 12, 16, a});
        DrawText(title, bx, by, 10, Color{255, 215, 0, a});
    }
}

} // namespace App
