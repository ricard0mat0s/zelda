#pragma once

#include "core/Dungeon.hpp"
#include "core/Player.hpp"
#include "core/Projectile.hpp"
#include "render/UIRenderer.hpp"
#include "render/DialogueBox.hpp"
#include "raylib.h"
#include <vector>

namespace App {

class DungeonScene {
public:
    DungeonScene();

    void init();
    void reset();

    void enterDungeon(const Core::Player& overworldPlayer);

    bool wantsExitToOverworld() const { return m_wantsExitToOverworld; }
    void clearExitRequest() { m_wantsExitToOverworld = false; }

    const Core::Player& getPlayer() const { return m_player; }
    Core::Player& getPlayer() { return m_player; }

    const Core::Dungeon& getDungeon() const { return m_dungeon; }
    Core::Dungeon& getDungeon() { return m_dungeon; }

    void update(float dt);
    void render();

private:
    void renderRoom(const Core::DungeonRoom& room);

    Core::Dungeon m_dungeon;
    Core::Player m_player;
    Render::UIRenderer m_uiRenderer;
    Render::DialogueBox m_dialogueBox;

    std::vector<Core::Arrow> m_arrows;

    Camera2D m_camera{};
    float m_bannerTimer = 0.0f;
    static constexpr float BANNER_DURATION = 3.5f;

    bool m_wantsExitToOverworld = false;
};

} // namespace App
