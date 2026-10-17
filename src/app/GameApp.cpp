#include "app/GameApp.hpp"
#include "raylib.h"
#include "core/GameConfig.hpp"
#include "render/AssetManager.hpp"
#include "render/AudioPlayer.hpp"
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

    // Initialize asset and audio subsystems
    Render::AssetManager::instance().init();
    Render::AssetManager::instance().exportSpritesToDisk("assets/sprites");
    Render::AudioPlayer::instance().init();

    m_state = Core::GameState::Title;
    m_running = true;

    return true;
}

void GameApp::close() {
    if (m_running) {
        Render::AudioPlayer::instance().cleanup();
        Render::AssetManager::instance().cleanup();
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
            m_overworldScene.update(dt);
            if (m_overworldScene.wantsEnterDungeon()) {
                m_overworldScene.clearDungeonRequest();
                m_dungeonScene.enterDungeon(m_overworldScene.getPlayer());
                m_state = Core::GameState::Dungeon;
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                m_titleScene.reset();
                m_overworldScene.reset();
                m_dungeonScene.reset();
                m_state = Core::GameState::Title;
            }
            break;
        case Core::GameState::Dungeon:
            m_dungeonScene.update(dt);
            if (m_dungeonScene.wantsExitToOverworld()) {
                m_dungeonScene.clearExitRequest();
                m_overworldScene.returnFromDungeon(m_dungeonScene.getPlayer());
                m_state = Core::GameState::Overworld;
            }
            if (IsKeyPressed(KEY_BACKSPACE)) {
                m_titleScene.reset();
                m_overworldScene.reset();
                m_dungeonScene.reset();
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
            m_overworldScene.render();
            break;
        case Core::GameState::Dungeon:
            m_dungeonScene.render();
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
