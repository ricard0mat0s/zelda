#pragma once

#include "core/Dungeon.hpp"
#include "core/Player.hpp"
#include "core/Projectile.hpp"
#include "core/DungeonEnemies.hpp"
#include "core/BossGolem.hpp"
#include "core/ItemDrop.hpp"
#include "core/Shield.hpp"
#include "core/SaveSystem.hpp"
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
    void applySaveData(const Core::SaveData& data);
    void saveCheckpoint();

    bool wantsExitToOverworld() const { return m_wantsExitToOverworld; }
    void clearExitRequest() { m_wantsExitToOverworld = false; }

    bool isVictory() const { return m_victory; }

    const Core::Player& getPlayer() const { return m_player; }
    Core::Player& getPlayer() { return m_player; }

    const Core::Dungeon& getDungeon() const { return m_dungeon; }
    Core::Dungeon& getDungeon() { return m_dungeon; }

    const Core::BossGolem& getBoss() const { return m_boss; }

    void update(float dt);
    void render();

private:
    void renderRoom(const Core::DungeonRoom& room);
    void spawnArenaEnemies();

    Core::Dungeon m_dungeon;
    Core::Player m_player;
    Render::UIRenderer m_uiRenderer;
    Render::DialogueBox m_dialogueBox;

    std::vector<Core::Arrow> m_arrows;
    std::vector<Core::Bat> m_bats;
    std::vector<Core::Skeleton> m_skeletons;
    std::vector<Core::ItemDrop> m_itemDrops;

    Core::BossGolem m_boss;
    std::vector<Core::ItemDrop> m_bossDrops;
    Core::Vec2 m_roomEntrancePos{152.0f, 180.0f};
    Core::Direction m_roomEntranceFacing = Core::Direction::Up;
    float m_screenShakeTimer = 0.0f;
    float m_swordBounceSoundCooldown = 0.0f;
    bool m_victory = false;
    bool m_victoryOverlayOpen = false;

    Camera2D m_camera{};
    float m_bannerTimer = 0.0f;
    static constexpr float BANNER_DURATION = 3.5f;

    bool m_wantsExitToOverworld = false;
};

} // namespace App
