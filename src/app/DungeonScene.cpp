#include "app/DungeonScene.hpp"
#include "render/AssetManager.hpp"
#include "render/AudioPlayer.hpp"
#include "core/GameConfig.hpp"
#include <algorithm>
#include <cmath>

namespace App {

DungeonScene::DungeonScene() {
    init();
}

void DungeonScene::init() {
    m_dungeon.init();
    reset();
}

void DungeonScene::reset() {
    m_dungeon.reset();
    m_player = Core::Player(152.0f, 180.0f);
    m_player.setFacing(Core::Direction::Up);
    m_bannerTimer = 0.0f;
    m_wantsExitToOverworld = false;

    m_camera.offset = Vector2{0.0f, 0.0f};
    m_camera.target = Vector2{0.0f, 0.0f};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

void DungeonScene::enterDungeon(const Core::Player& overworldPlayer) {
    m_player.setHealth(overworldPlayer.getHealth());
    m_player.getInventory() = overworldPlayer.getInventory();
    m_player.setPosition(Core::Vec2{152.0f, 180.0f});
    m_player.setFacing(Core::Direction::Up);
    m_bannerTimer = 0.0f;
    m_wantsExitToOverworld = false;
}

void DungeonScene::update(float dt) {
    // 0. If dead, allow retry
    if (m_player.getHealth().isDead()) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_player.heal(6);
            m_player.setPosition(Core::Vec2{152.0f, 180.0f});
            m_player.setFacing(Core::Direction::Up);
            m_dungeon.setCurrentRoomId(1);
        }
        return;
    }

    m_bannerTimer += dt;

    // 1. If room transition is active, advance timer and lock player control
    if (m_dungeon.isTransitioning()) {
        m_dungeon.update(dt);
        if (!m_dungeon.isTransitioning()) {
            // Transition just finished
            m_player.setPosition(m_dungeon.getTargetLocalPlayerPos());
        }

        // Smooth camera following during pan
        Core::Vec2 cam = m_dungeon.getCameraOffset();
        m_camera.target = Vector2{cam.x, cam.y};
        return;
    }

    // 2. Attack trigger
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) {
        if (m_player.startAttack()) {
            Render::AudioPlayer::instance().playSwordSlash();
        }
    }

    // 3. Movement input
    Core::MovementInput input;
    input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    m_player.updateInDungeon(dt, input, m_dungeon);

    // 4. Pressure plate trigger
    if (m_dungeon.checkPlateTrigger(m_player.getWorldHitbox())) {
        Render::AudioPlayer::instance().playSecretChime();
    }

    // 5. Doorway transition trigger
    m_dungeon.checkDoorwayTransition(m_player.getWorldHitbox(), m_player.getFacing(), m_player.getPosition());

    // 6. Overworld stairs exit check (in Room 1)
    if (m_dungeon.getCurrentRoomId() == 1 &&
        m_player.getFacing() == Core::Direction::Down &&
        m_dungeon.checkStairsExit(m_player.getWorldHitbox())) {
        m_wantsExitToOverworld = true;
    }

    // 7. Snap camera to current room
    Core::Vec2 cam = m_dungeon.getCameraOffset();
    m_camera.target = Vector2{cam.x, cam.y};
}

void DungeonScene::renderRoom(const Core::DungeonRoom& room) {
    auto& assets = Render::AssetManager::instance();
    Texture2D floorTex = assets.getTexture("dungeon_floor");
    Texture2D wallTex = assets.getTexture("dungeon_wall");
    Texture2D doorClosedTex = assets.getTexture("dungeon_door_closed");
    Texture2D doorOpenTex = assets.getTexture("dungeon_door_open");
    Texture2D plateUpTex = assets.getTexture("pressure_plate_up");
    Texture2D plateDownTex = assets.getTexture("pressure_plate_down");
    Texture2D stairsTex = assets.getTexture("dungeon_stairs");

    constexpr int tileSize = Core::DungeonRoom::TILE_SIZE;
    float ox = room.gridX * Core::DungeonRoom::ROOM_WIDTH;
    float oy = room.gridY * Core::DungeonRoom::ROOM_HEIGHT;

    // 1. Draw floor
    for (int r = 0; r < Core::DungeonRoom::ROWS; ++r) {
        for (int c = 0; c < Core::DungeonRoom::COLS; ++c) {
            DrawTexture(floorTex, static_cast<int>(ox + c * tileSize), static_cast<int>(oy + r * tileSize), WHITE);
        }
    }

    // 2. Draw border walls and doors
    for (int r = 0; r < Core::DungeonRoom::ROWS; ++r) {
        for (int c = 0; c < Core::DungeonRoom::COLS; ++c) {
            bool isTop = (r == 0);
            bool isBottom = (r == Core::DungeonRoom::ROWS - 1);
            bool isLeft = (c == 0);
            bool isRight = (c == Core::DungeonRoom::COLS - 1);

            if (!isTop && !isBottom && !isLeft && !isRight) {
                continue;
            }

            int px = static_cast<int>(ox + c * tileSize);
            int py = static_cast<int>(oy + r * tileSize);

            // North Doorway
            if (isTop && (c == 9 || c == 10)) {
                Core::DoorState ds = room.getDoor(Core::Direction::Up);
                if (ds == Core::DoorState::Open) {
                    DrawTexture(doorOpenTex, px, py, WHITE);
                } else if (ds == Core::DoorState::Closed || ds == Core::DoorState::Locked) {
                    DrawTexture(doorClosedTex, px, py, WHITE);
                } else {
                    DrawTexture(wallTex, px, py, WHITE);
                }
                continue;
            }

            // South Doorway
            if (isBottom && (c == 9 || c == 10)) {
                Core::DoorState ds = room.getDoor(Core::Direction::Down);
                if (ds == Core::DoorState::Open) {
                    DrawTexture(doorOpenTex, px, py, WHITE);
                } else if (ds == Core::DoorState::Closed || ds == Core::DoorState::Locked) {
                    DrawTexture(doorClosedTex, px, py, WHITE);
                } else {
                    DrawTexture(wallTex, px, py, WHITE);
                }
                continue;
            }

            // West Doorway
            if (isLeft && (r == 6 || r == 7)) {
                Core::DoorState ds = room.getDoor(Core::Direction::Left);
                if (ds == Core::DoorState::Open) {
                    DrawTexture(doorOpenTex, px, py, WHITE);
                } else if (ds == Core::DoorState::Closed || ds == Core::DoorState::Locked) {
                    DrawTexture(doorClosedTex, px, py, WHITE);
                } else {
                    DrawTexture(wallTex, px, py, WHITE);
                }
                continue;
            }

            // East Doorway
            if (isRight && (r == 6 || r == 7)) {
                Core::DoorState ds = room.getDoor(Core::Direction::Right);
                if (ds == Core::DoorState::Open) {
                    DrawTexture(doorOpenTex, px, py, WHITE);
                } else if (ds == Core::DoorState::Closed || ds == Core::DoorState::Locked) {
                    DrawTexture(doorClosedTex, px, py, WHITE);
                } else {
                    DrawTexture(wallTex, px, py, WHITE);
                }
                continue;
            }

            // Standard Wall
            DrawTexture(wallTex, px, py, WHITE);
        }
    }

    // 3. Draw Pressure Plate if room has one
    if (room.hasPlate) {
        Texture2D pTex = room.plate.pressed ? plateDownTex : plateUpTex;
        int ppx = static_cast<int>(ox + room.plate.position.x - 8.0f);
        int ppy = static_cast<int>(oy + room.plate.position.y - 8.0f);
        DrawTexture(pTex, ppx, ppy, WHITE);
    }

    // 4. Draw Stairs if room has stairs
    if (room.hasStairs) {
        int spx = static_cast<int>(ox + room.stairsPos.x - 8.0f);
        int spy = static_cast<int>(oy + room.stairsPos.y - 8.0f);
        DrawTexture(stairsTex, spx, spy, WHITE);
    }
}

void DungeonScene::render() {
    auto& assets = Render::AssetManager::instance();

    // 1. World-space Dungeon Rendering
    BeginMode2D(m_camera);

    if (m_dungeon.isTransitioning()) {
        const auto& trans = m_dungeon.getTransition();
        const auto* rFrom = m_dungeon.getRoom(trans.fromRoomId);
        const auto* rTo = m_dungeon.getRoom(trans.toRoomId);
        if (rFrom) renderRoom(*rFrom);
        if (rTo) renderRoom(*rTo);
    } else {
        renderRoom(m_dungeon.getCurrentRoom());
    }

    // Render Hero sprite
    const auto& health = m_player.getHealth();
    Texture2D heroTex = assets.getTexture(m_player.getTextureName());

    Core::Vec2 heroWorldPos;
    if (m_dungeon.isTransitioning()) {
        heroWorldPos = m_dungeon.getTransitionPlayerPos();
    } else {
        const auto& room = m_dungeon.getCurrentRoom();
        heroWorldPos = Core::Dungeon::getRoomCameraCenter(room.gridX, room.gridY) + m_player.getPosition();
    }

    int px = static_cast<int>(heroWorldPos.x);
    int py = static_cast<int>(heroWorldPos.y);

    if (health.isDead()) {
        DrawTexture(heroTex, px, py, Color{255, 100, 100, 220});
    } else if (health.isInvulnerable()) {
        if (health.isFlickerVisible()) {
            DrawTexture(heroTex, px, py, Color{255, 255, 255, 140});
        }
    } else {
        DrawTexture(heroTex, px, py, WHITE);
    }

    // Render sword slash animation
    if (m_player.isAttacking()) {
        Texture2D slashTex = assets.getTexture(m_player.getSlashTextureName());
        int sx = px;
        int sy = py;
        switch (m_player.getFacing()) {
            case Core::Direction::Down:  sy += 14; break;
            case Core::Direction::Up:    sy -= 14; break;
            case Core::Direction::Left:  sx -= 14; break;
            case Core::Direction::Right: sx += 14; break;
        }
        DrawTexture(slashTex, sx, sy, WHITE);
    }

    EndMode2D();

    // 2. Screen-space HUD & Overlays
    m_uiRenderer.renderHearts(m_player.getHealth());

    if (health.isDead()) {
        m_uiRenderer.renderGameOver();
    }

    // Location Banner: "THE SUNKEN CRYPT"
    if (m_bannerTimer < BANNER_DURATION) {
        float alpha = 1.0f;
        if (m_bannerTimer > BANNER_DURATION - 1.0f) {
            alpha = (BANNER_DURATION - m_bannerTimer);
        }
        unsigned char a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);

        const char* title = "THE SUNKEN CRYPT";
        int textW = MeasureText(title, 10);
        int bx = (Core::GameConfig::VIRTUAL_WIDTH - textW) / 2;
        int by = 18;

        DrawText(title, bx + 1, by + 1, 10, Color{11, 12, 16, a});
        DrawText(title, bx, by, 10, Color{100, 200, 255, a});

        std::string roomSub = m_dungeon.getCurrentRoom().name;
        int subW = MeasureText(roomSub.c_str(), 8);
        int subX = (Core::GameConfig::VIRTUAL_WIDTH - subW) / 2;
        int subY = by + 12;

        DrawText(roomSub.c_str(), subX + 1, subY + 1, 8, Color{11, 12, 16, a});
        DrawText(roomSub.c_str(), subX, subY, 8, Color{220, 220, 220, a});
    }
}

} // namespace App
