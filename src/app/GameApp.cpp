#include "app/GameApp.hpp"
#include "raylib.h"
#include "core/GameConfig.hpp"
#include <iostream>

namespace App {

GameApp::GameApp() = default;

GameApp::~GameApp() {
    close();
}

bool GameApp::init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    const int windowW = Core::GameConfig::VIRTUAL_WIDTH * Core::GameConfig::DEFAULT_SCALE;
    const int windowH = Core::GameConfig::VIRTUAL_HEIGHT * Core::GameConfig::DEFAULT_SCALE;

    InitWindow(windowW, windowH, "The Legend of Arcade Hero (Raylib C++)");
    SetWindowMinSize(Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT);
    SetTargetFPS(Core::GameConfig::TARGET_FPS);
    InitAudioDevice();

    m_canvas.init();
    m_state = Core::GameState::Title;
    m_running = true;

    return true;
}

void GameApp::close() {
    if (m_running) {
        m_canvas.close();
        CloseAudioDevice();
        CloseWindow();
        m_running = false;
    }
}

void GameApp::update(float dt) {
    switch (m_state) {
        case Core::GameState::Title:
            m_titleScene.update(dt);
            if (m_titleScene.isFinished()) {
                m_state = Core::GameState::Overworld;
            }
            break;
        case Core::GameState::Overworld:
            // Will run Overworld logic (Milestone 3)
            // Allow ESC or Backspace to return to Title for testing
            if (IsKeyPressed(KEY_BACKSPACE)) {
                m_titleScene.reset();
                m_state = Core::GameState::Title;
            }
            break;
        default:
            break;
    }
}

void GameApp::render() {
    m_canvas.beginVirtual();

    switch (m_state) {
        case Core::GameState::Title:
            m_titleScene.render();
            break;
        case Core::GameState::Overworld:
            // Placeholder overworld scene until Milestone 3
            DrawRectangle(0, 0, Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT, Color{45, 106, 79, 255}); // Forest green
            DrawText("OVERWORLD SCENE", 100, 100, 10, WHITE);
            DrawText("Press BACKSPACE to return to Title", 50, 130, 10, RAYWHITE);
            break;
        default:
            break;
    }

    m_canvas.endVirtual();

    // Draw to main screen scaled
    BeginDrawing();
    ClearBackground(BLACK);
    m_canvas.drawScaled();
    EndDrawing();
}

void GameApp::run() {
    while (!WindowShouldClose() && m_running) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // Cap large frame delta spikes

        update(dt);
        render();
    }
}

} // namespace App
