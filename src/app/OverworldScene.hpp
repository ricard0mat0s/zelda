#pragma once

#include "core/MapData.hpp"
#include "core/Player.hpp"
#include "core/Slime.hpp"
#include "core/ItemDrop.hpp"
#include "core/Shield.hpp"
#include "core/Projectile.hpp"
#include "core/SaveSystem.hpp"
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
    void update(float dt, const Core::GameInput& input);
    void render();

    const Core::Player& getPlayer() const { return m_player; }
    Core::Player& getPlayer() { return m_player; }

    const Core::MapData& getMap() const { return m_map; }
    const Render::DialogueBox& getDialogueBox() const { return m_dialogueBox; }
    bool shrineHasSword() const { return m_shrineHasSword; }
    Core::Vec2 getShrinePos() const { return m_shrinePos; }
    const std::vector<Core::Slime>& getSlimes() const { return m_slimes; }
    const std::vector<Core::ItemDrop>& getItemDrops() const { return m_itemDrops; }
    const std::vector<Core::Arrow>& getArrows() const { return m_arrows; }

    bool wantsEnterDungeon() const { return m_wantsEnterDungeon; }
    void clearDungeonRequest() { m_wantsEnterDungeon = false; }
    void returnFromDungeon(const Core::Player& dungeonPlayer);
    void applySaveData(const Core::SaveData& data);
    void snapCamera();

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
    std::vector<Core::Arrow> m_arrows;

    bool m_wantsEnterDungeon = false;
};

} // namespace App
