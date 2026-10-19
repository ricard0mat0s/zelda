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
    m_arrows.clear();
    m_dialogueBox.close();

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
    m_arrows.clear();
    m_dialogueBox.close();
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

    // 1. Dialogue box handling takes highest priority
    if (m_dialogueBox.isOpen()) {
        m_dialogueBox.update(dt);
        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_dialogueBox.advance();
        }
        return;
    }

    m_bannerTimer += dt;

    // 2. If room transition is active, advance timer and lock player control
    if (m_dungeon.isTransitioning()) {
        m_dungeon.update(dt);
        if (!m_dungeon.isTransitioning()) {
            m_player.setPosition(m_dungeon.getTargetLocalPlayerPos());
            m_arrows.clear(); // Clear arrows across transitions
        }

        // Smooth camera following during pan
        Core::Vec2 cam = m_dungeon.getCameraOffset();
        m_camera.target = Vector2{cam.x, cam.y};
        return;
    }

    // 3. Interact with treasure chest (Room 3)
    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
        if (m_dungeon.checkChestInteract(m_player.getPosition(), m_player.getFacing())) {
            m_player.getInventory().obtainBow();
            Render::AudioPlayer::instance().playChestFanfare();
            m_dialogueBox.show("Found the Bow & Quiver! Press [X] to fire arrows across chasms.");
            return;
        }
    }

    // 4. Primary sword attack
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) {
        if (m_player.startAttack()) {
            Render::AudioPlayer::instance().playSwordSlash();
        }
    }

    // 5. Secondary weapon: Bow & Arrow firing
    if (IsKeyPressed(KEY_X)) {
        if (m_player.getInventory().hasBow && !m_player.isAttacking()) {
            Core::Vec2 spawnPos = m_player.getPosition();
            switch (m_player.getFacing()) {
                case Core::Direction::Down:  spawnPos.y += 10.0f; break;
                case Core::Direction::Up:    spawnPos.y -= 10.0f; break;
                case Core::Direction::Left:  spawnPos.x -= 10.0f; break;
                case Core::Direction::Right: spawnPos.x += 10.0f; break;
            }
            m_arrows.emplace_back(spawnPos, m_player.getFacing(), 240.0f);
            Render::AudioPlayer::instance().playArrowShoot();
        }
    }

    // 6. Update active arrow projectiles
    for (auto& arrow : m_arrows) {
        if (!arrow.isActive()) continue;
        arrow.update(dt);

        // Check projectile collision with walls / obstacles
        if (m_dungeon.checkProjectileCollision(arrow.getHitbox())) {
            arrow.deactivate();
            continue;
        }

        // Check collision with crystal switch in current room
        if (m_dungeon.checkCrystalSwitchHit(arrow.getHitbox())) {
            Render::AudioPlayer::instance().playSecretChime();
            arrow.deactivate();
            continue;
        }
    }
    std::erase_if(m_arrows, [](const Core::Arrow& a) { return !a.isActive(); });

    // 7. Movement input
    Core::MovementInput input;
    input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    m_player.updateInDungeon(dt, input, m_dungeon);

    // 8. Pressure plate trigger
    if (m_dungeon.checkPlateTrigger(m_player.getWorldHitbox())) {
        Render::AudioPlayer::instance().playSecretChime();
    }

    // 9. Doorway transition trigger
    m_dungeon.checkDoorwayTransition(m_player.getWorldHitbox(), m_player.getFacing(), m_player.getPosition());

    // 10. Overworld stairs exit check (in Room 1)
    if (m_dungeon.getCurrentRoomId() == 1 &&
        m_player.getFacing() == Core::Direction::Down &&
        m_dungeon.checkStairsExit(m_player.getWorldHitbox())) {
        m_wantsExitToOverworld = true;
    }

    // 11. Snap camera to current room
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
    Texture2D chasmTex = assets.getTexture("chasm");
    Texture2D switchBlueTex = assets.getTexture("crystal_switch_blue");
    Texture2D switchActiveTex = assets.getTexture("crystal_switch_active");
    Texture2D chestClosedTex = assets.getTexture("chest_closed");
    Texture2D chestOpenTex = assets.getTexture("chest_open");

    constexpr int tileSize = Core::DungeonRoom::TILE_SIZE;
    float ox = room.gridX * Core::DungeonRoom::ROOM_WIDTH;
    float oy = room.gridY * Core::DungeonRoom::ROOM_HEIGHT;

    // 1. Draw floor
    for (int r = 0; r < Core::DungeonRoom::ROWS; ++r) {
        for (int c = 0; c < Core::DungeonRoom::COLS; ++c) {
            DrawTexture(floorTex, static_cast<int>(ox + c * tileSize), static_cast<int>(oy + r * tileSize), WHITE);
        }
    }

    // 2. Draw chasm pits (if room has any)
    for (const auto& pit : room.chasmPits) {
        int startC = static_cast<int>(pit.left() / tileSize);
        int endC = static_cast<int>(pit.right() / tileSize);
        int startR = static_cast<int>(pit.top() / tileSize);
        int endR = static_cast<int>(pit.bottom() / tileSize);
        for (int r = startR; r < endR; ++r) {
            for (int c = startC; c < endC; ++c) {
                DrawTexture(chasmTex, static_cast<int>(ox + c * tileSize), static_cast<int>(oy + r * tileSize), WHITE);
            }
        }
    }

    // 3. Draw border walls and doors
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

    // 4. Draw Pressure Plate if room has one
    if (room.hasPlate) {
        Texture2D pTex = room.plate.pressed ? plateDownTex : plateUpTex;
        int ppx = static_cast<int>(ox + room.plate.position.x - 8.0f);
        int ppy = static_cast<int>(oy + room.plate.position.y - 8.0f);
        DrawTexture(pTex, ppx, ppy, WHITE);
    }

    // 5. Draw Stairs if room has stairs
    if (room.hasStairs) {
        int spx = static_cast<int>(ox + room.stairsPos.x - 8.0f);
        int spy = static_cast<int>(oy + room.stairsPos.y - 8.0f);
        DrawTexture(stairsTex, spx, spy, WHITE);
    }

    // 6. Draw Crystal Switch if room has one
    if (room.hasCrystalSwitch) {
        Texture2D swTex = room.crystalSwitch.active ? switchActiveTex : switchBlueTex;
        int swx = static_cast<int>(ox + room.crystalSwitch.position.x - 8.0f);
        int swy = static_cast<int>(oy + room.crystalSwitch.position.y - 8.0f);
        DrawTexture(swTex, swx, swy, WHITE);
    }

    // 7. Draw Treasure Chest if room has one
    if (room.hasChest) {
        Texture2D cTex = room.chest.opened ? chestOpenTex : chestClosedTex;
        int cx = static_cast<int>(ox + room.chest.position.x - 8.0f);
        int cy = static_cast<int>(oy + room.chest.position.y - 8.0f);
        DrawTexture(cTex, cx, cy, WHITE);

        // Floating interaction hint if unopened and in range
        if (!room.chest.opened && room.chest.canOpen(m_player.getPosition(), m_player.getFacing())) {
            const char* prompt = "[E] OPEN";
            int pw = MeasureText(prompt, 8);
            DrawRectangle(cx + 8 - pw / 2 - 2, cy - 12, pw + 4, 10, Color{11, 12, 16, 200});
            DrawText(prompt, cx + 8 - pw / 2, cy - 11, 8, Color{250, 204, 21, 255});
        }
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

    // Render active Arrows
    const auto& curRoom = m_dungeon.getCurrentRoom();
    float rx = curRoom.gridX * Core::DungeonRoom::ROOM_WIDTH;
    float ry = curRoom.gridY * Core::DungeonRoom::ROOM_HEIGHT;
    for (const auto& arrow : m_arrows) {
        if (arrow.isActive()) {
            Texture2D arrowTex = assets.getTexture(arrow.getTextureName());
            int ax = static_cast<int>(rx + arrow.getPosition().x);
            int ay = static_cast<int>(ry + arrow.getPosition().y);
            DrawTexture(arrowTex, ax, ay, WHITE);
        }
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
    m_uiRenderer.renderInventory(m_player.getInventory());

    if (health.isDead()) {
        m_uiRenderer.renderGameOver();
    }

    // Dialogue Box overlay (e.g. Bow obtained)
    if (m_dialogueBox.isOpen()) {
        m_dialogueBox.render();
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
