#include "app/OverworldScene.hpp"
#include "render/AssetManager.hpp"
#include "render/AudioPlayer.hpp"
#include "core/GameConfig.hpp"
#include <algorithm>
#include <cmath>

namespace App {

OverworldScene::OverworldScene() {
    init();
}

void OverworldScene::init() {
    reset();
}

void OverworldScene::reset() {
    m_player = Core::Player(200.0f, 280.0f);
    m_bannerTimer = 0.0f;
    m_shrineHasSword = true;
    m_wantsEnterDungeon = false;

    // Spawn green slimes in forest clearings
    m_slimes.clear();
    m_slimes.emplace_back(140.0f, 200.0f);
    m_slimes.emplace_back(270.0f, 260.0f);
    m_slimes.emplace_back(360.0f, 180.0f);
    m_slimes.emplace_back(180.0f, 350.0f);
    m_slimes.emplace_back(420.0f, 250.0f);

    m_itemDrops.clear();
    m_arrows.clear();

    m_camera.offset = Vector2{0.0f, 0.0f};
    m_camera.target = Vector2{0.0f, 0.0f};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

void OverworldScene::update(float dt) {
    // 0. If dead, allow retry (respawn at clearing with retained inventory)
    if (m_player.getHealth().isDead()) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_player.heal(m_player.getHealth().getMaxHP());
            m_player.setPosition(Core::Vec2{200.0f, 280.0f});
            m_player.setFacing(Core::Direction::Down);
            m_arrows.clear();
        }
        return;
    }

    // 1. Dialogue box handling takes highest priority
    if (m_dialogueBox.isOpen()) {
        m_dialogueBox.update(dt);
        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            m_dialogueBox.advance();
        }
        return; // Freeze movement and attack while reading
    }

    // Debug testing controls for health verification
    if (IsKeyPressed(KEY_H)) {
        auto res = m_player.takeDamage(1);
        if (res.damaged) {
            Render::AudioPlayer::instance().playPlayerHurt();
        }
    }
    if (IsKeyPressed(KEY_J)) {
        m_player.heal(1);
    }

    // 2. Shrine proximity check
    float dx = (m_player.getPosition().x + 8.0f) - m_shrinePos.x;
    float dy = (m_player.getPosition().y + 8.0f) - m_shrinePos.y;
    bool nearShrine = (std::sqrt(dx * dx + dy * dy) <= 26.0f);

    if (nearShrine && (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER))) {
        if (m_shrineHasSword) {
            m_dialogueBox.show("It's dangerous to go alone! Take this.", [this]() {
                m_shrineHasSword = false;
                m_player.getInventory().obtainSword();
                Render::AudioPlayer::instance().playSecretChime();

                // Auto-save checkpoint upon obtaining the Sword
                Core::SaveData save;
                Core::SaveSystem::loadFromFile(save);
                save.maxHearts = m_player.getHealth().getMaxHearts();
                save.hasSword = true;
                save.hasBow = m_player.getInventory().hasBow;
                save.keys = m_player.getInventory().keys;
                save.inDungeon = false;
                save.respawnPos = Core::Vec2{200.0f, 204.0f};
                save.respawnFacing = Core::Direction::Down;
                Core::SaveSystem::saveToFile(save);
            });
        } else {
            m_dialogueBox.show("The ancient stone pedestal stands quiet in the glade.");
        }
        return;
    }

    // 3. Attack input (SPACE or Z)
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z)) {
        if (m_player.startAttack()) {
            Render::AudioPlayer::instance().playSwordSlash();
        }
    }

    // Secondary weapon: Bow & Arrow (X)
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

    // 4. Process movement input
    Core::MovementInput input;
    input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    m_player.update(dt, input, m_map);

    // Update arrows
    for (auto& arrow : m_arrows) {
        if (!arrow.isActive()) continue;
        arrow.update(dt);

        // Check collision with obstacles / map bounds
        if (m_map.checkRectCollision(arrow.getHitbox())) {
            arrow.deactivate();
            continue;
        }

        // Check collision with slimes
        for (auto& slime : m_slimes) {
            if (!slime.isDead() && arrow.getHitbox().overlaps(slime.getHitbox())) {
                Core::Vec2 kb = Core::calculateKnockback(arrow.getPosition(), slime.getPosition(), 140.0f);
                if (slime.takeDamage(arrow.getDamage(), kb)) {
                    if (slime.isDead()) {
                        Render::AudioPlayer::instance().playEnemyDefeat();
                        if ((static_cast<int>(slime.getPosition().x * 3 + slime.getPosition().y * 7) % 10) < 4) {
                            Core::ItemDrop drop;
                            drop.pos = slime.getPosition();
                            drop.type = Core::ItemType::HeartRecovery;
                            drop.active = true;
                            m_itemDrops.push_back(drop);
                        }
                    } else {
                        Render::AudioPlayer::instance().playHit();
                    }
                }
                arrow.deactivate();
                break;
            }
        }
    }
    std::erase_if(m_arrows, [](const Core::Arrow& a) { return !a.isActive(); });

    // 5. Update Slimes and Combat Interactions
    for (auto& slime : m_slimes) {
        if (slime.isDead()) continue;

        slime.update(dt, m_player.getPosition(), m_map);

        // Sword collision against slime
        if (m_player.isAttacking()) {
            Core::Rect swordBox = m_player.getCurrentSwordHitbox();
            if (swordBox.overlaps(slime.getHitbox())) {
                Core::Vec2 kb = Core::calculateKnockback(m_player.getPosition(), slime.getPosition(), 150.0f);
                if (slime.takeDamage(1, kb)) {
                    if (slime.isDead()) {
                        Render::AudioPlayer::instance().playEnemyDefeat();
                        // 35% chance to drop recovery heart
                        if ((static_cast<int>(slime.getPosition().x * 3 + slime.getPosition().y * 7) % 10) < 4) {
                            Core::ItemDrop drop;
                            drop.pos = slime.getPosition();
                            drop.type = Core::ItemType::HeartRecovery;
                            drop.active = true;
                            m_itemDrops.push_back(drop);
                        }
                    } else {
                        Render::AudioPlayer::instance().playHit();
                    }
                }
            }
        }

        // Slime collision with Player
        if (!slime.isDead() && slime.getHitbox().overlaps(m_player.getWorldHitbox())) {
            // Check Frontal Shield Defense!
            if (Core::isAttackBlockedByShield(m_player.getPosition(), m_player.getFacing(), slime.getPosition())) {
                Render::AudioPlayer::instance().playHit();
                Core::Vec2 repel = Core::calculateKnockback(m_player.getPosition(), slime.getPosition(), 90.0f);
                slime.takeDamage(0, repel); // Push enemy back without damaging
            } else {
                auto res = m_player.takeDamage(1); // 1 HP (half a heart)
                if (res.damaged) {
                    Render::AudioPlayer::instance().playPlayerHurt();
                }
            }
        }
    }

    // Update and collect Item Drops
    for (auto& drop : m_itemDrops) {
        if (!drop.active) continue;
        drop.update(dt);
        if (drop.getHitbox().overlaps(m_player.getWorldHitbox())) {
            if (drop.type == Core::ItemType::HeartRecovery) {
                m_player.heal(2); // 1 full heart
                Render::AudioPlayer::instance().playSecretChime();
                drop.active = false;
            }
        }
    }

    // 6. Check Dungeon entrance trigger at Northern Ruins (col: 20, row: 2, pos: 320, 32)
    Core::Rect entranceTrigger{314.0f, 26.0f, 12.0f, 12.0f};
    if (m_player.getWorldHitbox().overlaps(entranceTrigger) && m_player.getFacing() == Core::Direction::Up) {
        m_wantsEnterDungeon = true;
    }

    // 7. Camera tracking clamped to world bounds
    const float halfVW = Core::GameConfig::VIRTUAL_WIDTH * 0.5f;
    const float halfVH = Core::GameConfig::VIRTUAL_HEIGHT * 0.5f;
    const float maxX = static_cast<float>(Core::GameConfig::WORLD_WIDTH - Core::GameConfig::VIRTUAL_WIDTH);
    const float maxY = static_cast<float>(Core::GameConfig::WORLD_HEIGHT - Core::GameConfig::VIRTUAL_HEIGHT);

    float targetX = std::clamp(m_player.getPosition().x + 8.0f - halfVW, 0.0f, maxX);
    float targetY = std::clamp(m_player.getPosition().y + 8.0f - halfVH, 0.0f, maxY);

    // Smooth camera tracking
    m_camera.target.x += (targetX - m_camera.target.x) * std::min(1.0f, dt * 10.0f);
    m_camera.target.y += (targetY - m_camera.target.y) * std::min(1.0f, dt * 10.0f);

    // 8. Banner timer
    m_bannerTimer += dt;
}

void OverworldScene::render() {
    auto& assets = Render::AssetManager::instance();

    // 1. World pass inside camera
    BeginMode2D(m_camera);

    // Render ground and obstacle tilemaps
    m_mapRenderer.render(m_map, m_camera);

    // Render Elder Shrine Pedestal
    const char* shrineTexName = m_shrineHasSword ? "shrine_pedestal_sword" : "shrine_pedestal_empty";
    Texture2D shrineTex = assets.getTexture(shrineTexName);
    DrawTexture(shrineTex, static_cast<int>(m_shrinePos.x - 8.0f), static_cast<int>(m_shrinePos.y - 8.0f), WHITE);

    // Proximity prompt above pedestal
    float dx = (m_player.getPosition().x + 8.0f) - m_shrinePos.x;
    float dy = (m_player.getPosition().y + 8.0f) - m_shrinePos.y;
    bool nearShrine = (std::sqrt(dx * dx + dy * dy) <= 26.0f);
    if (nearShrine && !m_dialogueBox.isOpen()) {
        const char* prompt = "[E] EXAMINE";
        int pw = MeasureText(prompt, 7);
        int px = static_cast<int>(m_shrinePos.x) - pw / 2;
        int py = static_cast<int>(m_shrinePos.y) - 18;
        DrawText(prompt, px + 1, py + 1, 7, Color{11, 12, 16, 255});
        DrawText(prompt, px, py, 7, Color{255, 215, 0, 255});
    }

    // Render active Item Drops
    Texture2D heartTex = assets.getTexture("heart_full");
    for (const auto& drop : m_itemDrops) {
        if (drop.active && !drop.isBlinking()) {
            DrawTexture(heartTex, static_cast<int>(drop.pos.x), static_cast<int>(drop.pos.y), WHITE);
        }
    }

    // Render Slimes
    for (const auto& slime : m_slimes) {
        if (slime.isDead()) continue;
        const char* slimeTex = (slime.getAnimFrame() == 0) ? "slime_0" : "slime_1";
        Color tint = slime.isHurt() ? Color{255, 120, 120, 255} : WHITE;
        DrawTexture(assets.getTexture(slimeTex), static_cast<int>(slime.getPosition().x), static_cast<int>(slime.getPosition().y), tint);
    }

    // Render Hero sprite with invulnerability flicker
    const auto& health = m_player.getHealth();
    Texture2D heroTex = assets.getTexture(m_player.getTextureName());
    int px = static_cast<int>(m_player.getPosition().x);
    int py = static_cast<int>(m_player.getPosition().y);

    if (health.isDead()) {
        DrawTexture(heroTex, px, py, Color{255, 100, 100, 220});
    } else if (health.isInvulnerable()) {
        if (health.isFlickerVisible()) {
            DrawTexture(heroTex, px, py, Color{255, 255, 255, 140});
        }
    } else {
        DrawTexture(heroTex, px, py, WHITE);
    }

    // Render sword slash animation in front of player
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

    // Render active arrows
    for (const auto& arrow : m_arrows) {
        if (arrow.isActive()) {
            Texture2D arrowTex = assets.getTexture(arrow.getTextureName());
            int ax = static_cast<int>(arrow.getPosition().x);
            int ay = static_cast<int>(arrow.getPosition().y);
            DrawTexture(arrowTex, ax, ay, WHITE);
        }
    }

    EndMode2D();

    // 2. Screen-space HUD & Overlays
    m_uiRenderer.renderHearts(m_player.getHealth());
    m_uiRenderer.renderInventory(m_player.getInventory());

    if (health.isDead()) {
        m_uiRenderer.renderGameOver();
    }

    // Dialogue Box overlay
    if (m_dialogueBox.isOpen()) {
        m_dialogueBox.render();
    }

    // Location Banner
    if (m_bannerTimer < BANNER_DURATION) {
        float alpha = 1.0f;
        if (m_bannerTimer > BANNER_DURATION - 1.0f) {
            alpha = (BANNER_DURATION - m_bannerTimer);
        }
        unsigned char a = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f);

        const char* title = "HYLIA GLADE";
        int textW = MeasureText(title, 10);
        int bx = (Core::GameConfig::VIRTUAL_WIDTH - textW) / 2;
        int by = 20;

        DrawText(title, bx + 1, by + 1, 10, Color{11, 12, 16, a});
        DrawText(title, bx, by, 10, Color{255, 215, 0, a});
    }
}

void OverworldScene::returnFromDungeon(const Core::Player& dungeonPlayer) {
    m_player.setHealth(dungeonPlayer.getHealth());
    m_player.getInventory() = dungeonPlayer.getInventory();
    m_player.setPosition(Core::Vec2{312.0f, 54.0f});
    m_player.setFacing(Core::Direction::Down);
    m_bannerTimer = 0.0f;
    m_wantsEnterDungeon = false;
}

void OverworldScene::applySaveData(const Core::SaveData& data) {
    m_player.setHealth(Core::Health(data.maxHearts));
    m_player.getInventory().hasSword = data.hasSword;
    m_player.getInventory().hasBow = data.hasBow;
    m_player.getInventory().keys = data.keys;
    m_shrineHasSword = !data.hasSword;

    if (!data.inDungeon) {
        m_player.setPosition(data.respawnPos);
        m_player.setFacing(data.respawnFacing);
    }
}

} // namespace App
