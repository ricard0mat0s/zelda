#pragma once

#include "core/MapData.hpp"
#include "core/Player.hpp"
#include "core/Slime.hpp"
#include "core/ItemDrop.hpp"
#include "core/Shield.hpp"
#include "render/MapRenderer.hpp"
#include "render/UIRenderer.hpp"
#include "render/DialogueBox.hpp"
#include "raylib.h"
#include <vector>

namespace App {

class OverworldScene {
public:
    OverworldScene();

    void init();
    void reset();

    void update(float dt);
    void render();

    const Core::Player& getPlayer() const { return m_player; }
    Core::Player& getPlayer() { return m_player; }

    const Core::MapData& getMap() const { return m_map; }

    bool wantsEnterDungeon() const { return m_wantsEnterDungeon; }
    void clearDungeonRequest() { m_wantsEnterDungeon = false; }
    void returnFromDungeon(const Core::Player& dungeonPlayer);

private:
    Core::MapData m_map;
    Core::Player m_player;
    Render::MapRenderer m_mapRenderer;
    Render::UIRenderer m_uiRenderer;
    Render::DialogueBox m_dialogueBox;

    Camera2D m_camera{};
    float m_bannerTimer = 0.0f;
    static constexpr float BANNER_DURATION = 3.5f;

    // Shrine position and state
    Core::Vec2 m_shrinePos{200.0f, 180.0f};
    bool m_shrineHasSword = true;

    // Overworld entities and drops
    std::vector<Core::Slime> m_slimes;
    std::vector<Core::ItemDrop> m_itemDrops;

    bool m_wantsEnterDungeon = false;
};

} // namespace App
