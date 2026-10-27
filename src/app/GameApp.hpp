#pragma once

#include "core/types.hpp"
#include "render/RenderUtils.hpp"
#include "app/TitleScene.hpp"
#include "app/OverworldScene.hpp"
#include "app/DungeonScene.hpp"
#include <string>

namespace App {

class GameApp {
public:
    GameApp();
    ~GameApp();

    bool init(bool hiddenWindow = false);
    void run();
    void close();

    // Agent & Harness interface
    void stepFrame(const Core::GameInput& input, float dt = 1.0f / 60.0f);
    void renderToCanvas();
    bool saveScreenshot(const std::string& path, int scale = 3);
    std::string dumpStateJson() const;
    bool saveStateJson(const std::string& path) const;
    bool runHarnessScript(
        const std::string& script,
        const std::string& finalScreenshotPath = "",
        const std::string& finalStatePath = "",
        bool renderEveryFrame = false);

    Core::GameState getState() const { return m_state; }
    int getFrameCount() const { return m_frameCount; }

private:
    void update(float dt);
    void update(float dt, const Core::GameInput& input);
    void render();
    static Core::GameInput pollKeyboardInput();

    Core::GameState m_state = Core::GameState::Title;
    Render::VirtualCanvas m_canvas;
    TitleScene m_titleScene;
    OverworldScene m_overworldScene;
    DungeonScene m_dungeonScene;
    bool m_running = false;
    int m_frameCount = 0;
};

} // namespace App
