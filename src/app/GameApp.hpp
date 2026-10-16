#pragma once

#include "core/types.hpp"
#include "render/RenderUtils.hpp"
#include "app/TitleScene.hpp"
#include "app/OverworldScene.hpp"

namespace App {

class GameApp {
public:
    GameApp();
    ~GameApp();

    bool init();
    void run();
    void close();

private:
    void update(float dt);
    void render();

    Core::GameState m_state = Core::GameState::Title;
    Render::VirtualCanvas m_canvas;
    TitleScene m_titleScene;
    OverworldScene m_overworldScene;
    bool m_running = false;
};

} // namespace App
