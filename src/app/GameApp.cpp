#include "app/GameApp.hpp"
#include "raylib.h"
#include "core/GameConfig.hpp"
#include "core/SaveSystem.hpp"
#include "render/AssetManager.hpp"
#include "render/AudioPlayer.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace App {

namespace {

std::string escapeJson(const std::string& s) {
    std::ostringstream oss;
    for (char c : s) {
        switch (c) {
            case '"':  oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:   oss << c; break;
        }
    }
    return oss.str();
}

const char* doorStateToString(Core::DoorState s) {
    switch (s) {
        case Core::DoorState::None:   return "Wall";
        case Core::DoorState::Closed: return "Closed";
        case Core::DoorState::Open:   return "Open";
        case Core::DoorState::Locked: return "Locked";
    }
    return "Unknown";
}

const char* bossPhaseToString(Core::BossPhase p) {
    switch (p) {
        case Core::BossPhase::ArmoredStomp: return "ArmoredStomp";
        case Core::BossPhase::EyeCharging:  return "EyeCharging";
        case Core::BossPhase::Stunned:      return "Stunned";
        case Core::BossPhase::Dying:        return "Dying";
        case Core::BossPhase::Defeated:     return "Defeated";
    }
    return "Unknown";
}

const char* itemTypeToString(Core::ItemType t) {
    switch (t) {
        case Core::ItemType::HeartRecovery:  return "HeartRecovery";
        case Core::ItemType::SmallKey:       return "SmallKey";
        case Core::ItemType::HeartContainer: return "HeartContainer";
    }
    return "Unknown";
}

std::string trimString(const std::string& str) {
    const auto begin = str.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(begin, end - begin + 1);
}

} // namespace

GameApp::GameApp() = default;

GameApp::~GameApp() {
    close();
}

bool GameApp::init(bool hiddenWindow) {
    if (hiddenWindow) {
        SetTraceLogLevel(LOG_WARNING);
        SetConfigFlags(FLAG_WINDOW_HIDDEN);
    } else {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    }

    const int windowW = Core::GameConfig::VIRTUAL_WIDTH * Core::GameConfig::DEFAULT_SCALE;
    const int windowH = Core::GameConfig::VIRTUAL_HEIGHT * Core::GameConfig::DEFAULT_SCALE;

    InitWindow(windowW, windowH, "The Legend of Arcade Hero (Raylib C++)");
    SetWindowMinSize(Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT);
    if (!hiddenWindow) {
        SetTargetFPS(Core::GameConfig::TARGET_FPS);
    }
    InitAudioDevice();

    m_canvas.init();

    // Initialize asset and audio subsystems
    Render::AssetManager::instance().init();
    Render::AssetManager::instance().exportSpritesToDisk("assets/sprites");
    Render::AudioPlayer::instance().init();

    m_state = Core::GameState::Title;
    m_running = true;
    m_frameCount = 0;

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

Core::GameInput GameApp::pollKeyboardInput() {
    Core::GameInput input;
    input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    input.confirmPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER);
    input.interactPressed = IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER);
    input.attackPressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_Z);
    input.bowPressed = IsKeyPressed(KEY_X);
    input.newGamePressed = IsKeyPressed(KEY_N);
    input.backToTitlePressed = IsKeyPressed(KEY_BACKSPACE);
    input.debugDamagePressed = IsKeyPressed(KEY_H);
    input.debugHealPressed = IsKeyPressed(KEY_J);
    return input;
}

void GameApp::update(float dt) {
    update(dt, pollKeyboardInput());
}

void GameApp::update(float dt, const Core::GameInput& input) {
    ++m_frameCount;

    switch (m_state) {
        case Core::GameState::Title:
            m_titleScene.update(dt, input);
            if (m_titleScene.isFinished()) {
                if (m_titleScene.wantsContinue()) {
                    Core::SaveData save;
                    if (Core::SaveSystem::loadFromFile(save)) {
                        m_overworldScene.applySaveData(save);
                        m_dungeonScene.applySaveData(save);
                        m_state = save.inDungeon ? Core::GameState::Dungeon : Core::GameState::Overworld;
                        break;
                    }
                }
                m_overworldScene.reset();
                m_dungeonScene.reset();
                m_state = Core::GameState::Overworld;
            }
            break;
        case Core::GameState::Overworld:
            m_overworldScene.update(dt, input);
            if (m_overworldScene.wantsEnterDungeon()) {
                m_overworldScene.clearDungeonRequest();
                m_dungeonScene.enterDungeon(m_overworldScene.getPlayer());
                m_state = Core::GameState::Dungeon;
            }
            if (input.backToTitlePressed) {
                m_titleScene.reset();
                m_overworldScene.reset();
                m_dungeonScene.reset();
                m_state = Core::GameState::Title;
            }
            break;
        case Core::GameState::Dungeon:
            m_dungeonScene.update(dt, input);
            if (m_dungeonScene.wantsExitToOverworld()) {
                m_dungeonScene.clearExitRequest();
                m_overworldScene.returnFromDungeon(m_dungeonScene.getPlayer());
                m_state = Core::GameState::Overworld;
            }
            if (input.backToTitlePressed) {
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

void GameApp::stepFrame(const Core::GameInput& input, float dt) {
    update(dt, input);
}

void GameApp::renderToCanvas() {
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
}

void GameApp::render() {
    renderToCanvas();

    // Draw to main screen scaled
    BeginDrawing();
    ClearBackground(BLACK);
    m_canvas.drawScaled();
    EndDrawing();
}

bool GameApp::saveScreenshot(const std::string& path, int scale) {
    renderToCanvas();
    return m_canvas.saveScreenshot(path, scale);
}

std::string GameApp::dumpStateJson() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    oss << "{\n";
    oss << "  \"frame\": " << m_frameCount << ",\n";

    const char* sceneName = "Unknown";
    if (m_state == Core::GameState::Title) sceneName = "Title";
    else if (m_state == Core::GameState::Overworld) sceneName = "Overworld";
    else if (m_state == Core::GameState::Dungeon) sceneName = "Dungeon";
    oss << "  \"scene\": \"" << sceneName << "\",\n";

    if (m_state == Core::GameState::Title) {
        oss << "  \"title\": {\n";
        oss << "    \"hasSave\": " << (m_titleScene.hasSave() ? "true" : "false") << ",\n";
        oss << "    \"promptVisible\": " << (m_titleScene.isPromptVisible() ? "true" : "false") << ",\n";
        oss << "    \"finished\": " << (m_titleScene.isFinished() ? "true" : "false") << "\n";
        oss << "  }\n";
    } else {
        const Core::Player& player = (m_state == Core::GameState::Overworld)
            ? m_overworldScene.getPlayer()
            : m_dungeonScene.getPlayer();
        const Render::DialogueBox& dlg = (m_state == Core::GameState::Overworld)
            ? m_overworldScene.getDialogueBox()
            : m_dungeonScene.getDialogueBox();

        oss << "  \"player\": {\n";
        oss << "    \"pos\": {\"x\": " << player.getPosition().x << ", \"y\": " << player.getPosition().y << "},\n";
        oss << "    \"facing\": \"" << Core::directionToString(player.getFacing()) << "\",\n";
        oss << "    \"hp\": " << player.getHealth().getCurrentHP() << ",\n";
        oss << "    \"maxHp\": " << player.getHealth().getMaxHP() << ",\n";
        oss << "    \"dead\": " << (player.getHealth().isDead() ? "true" : "false") << ",\n";
        oss << "    \"invulnerable\": " << (player.getHealth().isInvulnerable() ? "true" : "false") << ",\n";
        oss << "    \"attacking\": " << (player.isAttacking() ? "true" : "false") << ",\n";
        oss << "    \"inventory\": {\n";
        oss << "      \"hasSword\": " << (player.getInventory().hasSword ? "true" : "false") << ",\n";
        oss << "      \"hasBow\": " << (player.getInventory().hasBow ? "true" : "false") << ",\n";
        oss << "      \"keys\": " << player.getInventory().keys << "\n";
        oss << "    }\n";
        oss << "  },\n";

        oss << "  \"dialogue\": {\n";
        oss << "    \"open\": " << (dlg.isOpen() ? "true" : "false") << ",\n";
        oss << "    \"complete\": " << (dlg.isTextComplete() ? "true" : "false") << ",\n";
        oss << "    \"text\": \"" << escapeJson(dlg.isOpen() ? dlg.getFullText() : "") << "\"\n";
        oss << "  },\n";

        if (m_state == Core::GameState::Overworld) {
            oss << "  \"overworld\": {\n";
            oss << "    \"shrinePos\": {\"x\": " << m_overworldScene.getShrinePos().x
                << ", \"y\": " << m_overworldScene.getShrinePos().y << "},\n";
            oss << "    \"shrineHasSword\": " << (m_overworldScene.shrineHasSword() ? "true" : "false") << ",\n";
            oss << "    \"dungeonEntrance\": {\"x\": 314.0, \"y\": 32.0},\n";
            oss << "    \"slimes\": [\n";
            bool firstSlime = true;
            for (size_t i = 0; i < m_overworldScene.getSlimes().size(); ++i) {
                const auto& s = m_overworldScene.getSlimes()[i];
                if (s.isDead()) continue;
                if (!firstSlime) oss << ",\n";
                firstSlime = false;
                oss << "      {\"index\": " << i
                    << ", \"pos\": {\"x\": " << s.getPosition().x << ", \"y\": " << s.getPosition().y << "}"
                    << ", \"hp\": " << s.getHP()
                    << ", \"hurt\": " << (s.isHurt() ? "true" : "false") << "}";
            }
            oss << "\n    ],\n";
            oss << "    \"drops\": [\n";
            bool firstDrop = true;
            for (const auto& d : m_overworldScene.getItemDrops()) {
                if (!d.active) continue;
                if (!firstDrop) oss << ",\n";
                firstDrop = false;
                oss << "      {\"type\": \"" << itemTypeToString(d.type)
                    << "\", \"pos\": {\"x\": " << d.pos.x << ", \"y\": " << d.pos.y << "}}";
            }
            oss << "\n    ]\n";
            oss << "  }\n";
        } else if (m_state == Core::GameState::Dungeon) {
            const auto& dungeon = m_dungeonScene.getDungeon();
            const auto& room = dungeon.getCurrentRoom();
            oss << "  \"dungeon\": {\n";
            oss << "    \"roomId\": " << room.id << ",\n";
            oss << "    \"roomName\": \"" << escapeJson(room.name) << "\",\n";
            oss << "    \"transitioning\": " << (dungeon.isTransitioning() ? "true" : "false") << ",\n";
            oss << "    \"victory\": " << (m_dungeonScene.isVictory() ? "true" : "false") << ",\n";
            oss << "    \"victoryOverlayOpen\": " << (m_dungeonScene.isVictoryOverlayOpen() ? "true" : "false") << ",\n";
            oss << "    \"doors\": {\n";
            oss << "      \"Up\": \"" << doorStateToString(room.getDoor(Core::Direction::Up)) << "\",\n";
            oss << "      \"Down\": \"" << doorStateToString(room.getDoor(Core::Direction::Down)) << "\",\n";
            oss << "      \"Left\": \"" << doorStateToString(room.getDoor(Core::Direction::Left)) << "\",\n";
            oss << "      \"Right\": \"" << doorStateToString(room.getDoor(Core::Direction::Right)) << "\"\n";
            oss << "    },\n";
            oss << "    \"roomFeatures\": {\n";
            oss << "      \"hasPlate\": " << (room.hasPlate ? "true" : "false")
                << ", \"platePressed\": " << (room.plate.pressed ? "true" : "false")
                << ", \"platePos\": {\"x\": " << room.plate.position.x << ", \"y\": " << room.plate.position.y << "},\n";
            oss << "      \"hasCrystalSwitch\": " << (room.hasCrystalSwitch ? "true" : "false")
                << ", \"crystalActive\": " << (room.crystalSwitch.active ? "true" : "false")
                << ", \"crystalPos\": {\"x\": " << room.crystalSwitch.position.x << ", \"y\": " << room.crystalSwitch.position.y << "},\n";
            oss << "      \"hasChest\": " << (room.hasChest ? "true" : "false")
                << ", \"chestOpened\": " << (room.chest.opened ? "true" : "false")
                << ", \"chestPos\": {\"x\": " << room.chest.position.x << ", \"y\": " << room.chest.position.y << "},\n";
            oss << "      \"isArena\": " << (room.isArena ? "true" : "false")
                << ", \"arenaLocked\": " << (room.arenaLocked ? "true" : "false")
                << ", \"arenaCleared\": " << (room.arenaCleared ? "true" : "false") << "\n";
            oss << "    },\n";

            // Enemies & drops in room
            oss << "    \"enemies\": [\n";
            bool firstEnemy = true;
            if (room.id == 4) {
                for (const auto& b : m_dungeonScene.getBats()) {
                    if (b.isDead()) continue;
                    if (!firstEnemy) oss << ",\n";
                    firstEnemy = false;
                    oss << "      {\"type\": \"Bat\", \"pos\": {\"x\": " << b.getPosition().x
                        << ", \"y\": " << b.getPosition().y << "}, \"hp\": " << b.getHP() << "}";
                }
                for (const auto& s : m_dungeonScene.getSkeletons()) {
                    if (s.isDead()) continue;
                    if (!firstEnemy) oss << ",\n";
                    firstEnemy = false;
                    oss << "      {\"type\": \"Skeleton\", \"pos\": {\"x\": " << s.getPosition().x
                        << ", \"y\": " << s.getPosition().y << "}, \"hp\": " << s.getHP() << "}";
                }
            } else if (room.id == 6) {
                const auto& boss = m_dungeonScene.getBoss();
                oss << "      {\"type\": \"BossGolem\", \"phase\": \"" << bossPhaseToString(boss.getPhase())
                    << "\", \"pos\": {\"x\": " << boss.getPosition().x << ", \"y\": " << boss.getPosition().y
                    << "}, \"hp\": " << boss.getHP() << ", \"maxHp\": " << boss.getMaxHP()
                    << ", \"defeated\": " << (boss.isDefeated() ? "true" : "false") << "}";
            }
            oss << "\n    ],\n";
            oss << "    \"drops\": [\n";
            bool firstDrop = true;
            for (const auto& d : m_dungeonScene.getItemDrops()) {
                if (!d.active) continue;
                if (!firstDrop) oss << ",\n";
                firstDrop = false;
                oss << "      {\"type\": \"" << itemTypeToString(d.type)
                    << "\", \"pos\": {\"x\": " << d.pos.x << ", \"y\": " << d.pos.y << "}}";
            }
            for (const auto& d : m_dungeonScene.getBossDrops()) {
                if (!d.active) continue;
                if (!firstDrop) oss << ",\n";
                firstDrop = false;
                oss << "      {\"type\": \"" << itemTypeToString(d.type)
                    << "\", \"pos\": {\"x\": " << d.pos.x << ", \"y\": " << d.pos.y << "}}";
            }
            oss << "\n    ]\n";
            oss << "  }\n";
        }
    }

    oss << "}\n";
    return oss.str();
}

bool GameApp::saveStateJson(const std::string& path) const {
    if (path.empty()) return false;
    std::ofstream out(path);
    if (!out.is_open()) return false;
    out << dumpStateJson();
    return true;
}

bool GameApp::runHarnessScript(
    const std::string& script,
    const std::string& finalScreenshotPath,
    const std::string& finalStatePath,
    bool renderEveryFrame)
{
    constexpr float kFixedDt = 1.0f / 60.0f;

    auto tick = [&](const Core::GameInput& in) {
        stepFrame(in, kFixedDt);
        if (renderEveryFrame) {
            render();
        }
    };

    // Split script by ';' or '\n'
    std::vector<std::string> commands;
    std::string current;
    for (char c : script) {
        if (c == ';' || c == '\n') {
            std::string trimmed = trimString(current);
            if (!trimmed.empty() && trimmed[0] != '#') {
                commands.push_back(trimmed);
            }
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    std::string lastTrimmed = trimString(current);
    if (!lastTrimmed.empty() && lastTrimmed[0] != '#') {
        commands.push_back(lastTrimmed);
    }

    for (const std::string& cmdLine : commands) {
        std::istringstream iss(cmdLine);
        std::string verb;
        iss >> verb;
        if (verb.empty()) continue;

        auto readCount = [&](int defaultFrames) {
            int n = defaultFrames;
            if (iss >> n) {
                return std::max(1, n);
            }
            return defaultFrames;
        };

        if (verb == "wait") {
            int n = readCount(1);
            for (int i = 0; i < n; ++i) tick(Core::GameInput{});
        } else if (verb == "up" || verb == "down" || verb == "left" || verb == "right" ||
                   verb == "up_left" || verb == "up_right" || verb == "down_left" || verb == "down_right") {
            int n = readCount(1);
            Core::GameInput in;
            in.up = (verb == "up" || verb == "up_left" || verb == "up_right");
            in.down = (verb == "down" || verb == "down_left" || verb == "down_right");
            in.left = (verb == "left" || verb == "up_left" || verb == "down_left");
            in.right = (verb == "right" || verb == "up_right" || verb == "down_right");
            for (int i = 0; i < n; ++i) tick(in);
        } else if (verb == "face") {
            std::string dir;
            iss >> dir;
            Core::Player* p = (m_state == Core::GameState::Overworld) ? &m_overworldScene.getPlayer()
                            : (m_state == Core::GameState::Dungeon)   ? &m_dungeonScene.getPlayer()
                            : nullptr;
            if (p) {
                if (dir == "up" || dir == "Up") p->setFacing(Core::Direction::Up);
                else if (dir == "down" || dir == "Down") p->setFacing(Core::Direction::Down);
                else if (dir == "left" || dir == "Left") p->setFacing(Core::Direction::Left);
                else if (dir == "right" || dir == "Right") p->setFacing(Core::Direction::Right);
            }
        } else if (verb == "move_to") {
            float tx = 0.0f, ty = 0.0f;
            int maxFrames = 300;
            iss >> tx >> ty;
            if (!(iss >> maxFrames)) maxFrames = 300;
            Core::GameState startScene = m_state;
            int startRoom = (m_state == Core::GameState::Dungeon) ? m_dungeonScene.getDungeon().getCurrentRoomId() : -1;
            for (int i = 0; i < maxFrames; ++i) {
                if (m_state != startScene) break;
                if (m_state == Core::GameState::Dungeon) {
                    if (m_dungeonScene.getDungeon().isTransitioning()) {
                        tick(Core::GameInput{});
                        continue;
                    }
                    if (m_dungeonScene.getDungeon().getCurrentRoomId() != startRoom) break;
                }
                const Core::Player* p = (m_state == Core::GameState::Overworld) ? &m_overworldScene.getPlayer()
                                      : (m_state == Core::GameState::Dungeon)   ? &m_dungeonScene.getPlayer()
                                      : nullptr;
                if (!p) break;
                float dx = tx - p->getPosition().x;
                float dy = ty - p->getPosition().y;
                if (std::abs(dx) <= 2.5f && std::abs(dy) <= 2.5f) break;
                Core::GameInput in;
                if (dx < -2.0f) in.left = true;
                else if (dx > 2.0f) in.right = true;
                if (dy < -2.0f) in.up = true;
                else if (dy > 2.0f) in.down = true;
                tick(in);
            }
        } else if (verb == "start" || verb == "confirm") {
            int n = readCount(2);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.confirmPressed = (i == 0);
                tick(in);
            }
        } else if (verb == "new_game") {
            int n = readCount(2);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                if (i == 0) {
                    if (m_titleScene.hasSave()) in.newGamePressed = true;
                    else in.confirmPressed = true;
                }
                tick(in);
            }
        } else if (verb == "interact") {
            int n = readCount(45);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.interactPressed = (i == 0);
                tick(in);
            }
        } else if (verb == "dismiss_dialogue") {
            for (int attempt = 0; attempt < 4; ++attempt) {
                const Render::DialogueBox* dlg = (m_state == Core::GameState::Overworld) ? &m_overworldScene.getDialogueBox()
                                               : (m_state == Core::GameState::Dungeon)   ? &m_dungeonScene.getDialogueBox()
                                               : nullptr;
                if (!dlg || !dlg->isOpen()) break;
                Core::GameInput press;
                press.interactPressed = true;
                press.confirmPressed = true;
                tick(press);
                for (int i = 0; i < 10; ++i) tick(Core::GameInput{});
            }
        } else if (verb == "attack") {
            int n = readCount(16);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.attackPressed = (i == 0);
                tick(in);
            }
        } else if (verb == "bow" || verb == "shoot") {
            int n = readCount(15);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.bowPressed = (i == 0);
                tick(in);
            }
        } else if (verb == "back") {
            int n = readCount(5);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.backToTitlePressed = (i == 0);
                tick(in);
            }
        } else if (verb == "hurt") {
            int n = readCount(5);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.debugDamagePressed = (i == 0);
                tick(in);
            }
        } else if (verb == "heal") {
            int n = readCount(5);
            for (int i = 0; i < n; ++i) {
                Core::GameInput in;
                in.debugHealPressed = (i == 0);
                tick(in);
            }
        } else if (verb == "teleport") {
            float x = 0.0f, y = 0.0f;
            if (iss >> x >> y) {
                if (m_state == Core::GameState::Overworld) {
                    m_overworldScene.getPlayer().setPosition(Core::Vec2{x, y});
                    m_overworldScene.snapCamera();
                } else if (m_state == Core::GameState::Dungeon) {
                    m_dungeonScene.getPlayer().setPosition(Core::Vec2{x, y});
                }
                tick(Core::GameInput{});
            }
        } else if (verb == "give_sword") {
            if (m_state == Core::GameState::Overworld) m_overworldScene.getPlayer().getInventory().obtainSword();
            else if (m_state == Core::GameState::Dungeon) m_dungeonScene.getPlayer().getInventory().obtainSword();
        } else if (verb == "give_bow") {
            if (m_state == Core::GameState::Overworld) m_overworldScene.getPlayer().getInventory().obtainBow();
            else if (m_state == Core::GameState::Dungeon) m_dungeonScene.getPlayer().getInventory().obtainBow();
        } else if (verb == "give_key") {
            if (m_state == Core::GameState::Overworld) m_overworldScene.getPlayer().getInventory().addKeys(1);
            else if (m_state == Core::GameState::Dungeon) m_dungeonScene.getPlayer().getInventory().addKeys(1);
        } else if (verb == "enter_dungeon") {
            m_dungeonScene.enterDungeon(m_overworldScene.getPlayer());
            m_state = Core::GameState::Dungeon;
            tick(Core::GameInput{});
        } else if (verb == "room") {
            int roomId = 1;
            if ((iss >> roomId) && m_state == Core::GameState::Dungeon) {
                m_dungeonScene.getDungeon().setCurrentRoomId(roomId);
                m_dungeonScene.getPlayer().setPosition(Core::Vec2{152.0f, 180.0f});
                tick(Core::GameInput{});
            }
        } else if (verb == "screenshot") {
            std::string path;
            iss >> path;
            if (!path.empty()) {
                saveScreenshot(path);
            }
        } else if (verb == "state") {
            std::string path;
            iss >> path;
            if (!path.empty()) {
                saveStateJson(path);
            }
        }
    }

    if (!finalScreenshotPath.empty()) {
        saveScreenshot(finalScreenshotPath);
    }
    if (!finalStatePath.empty()) {
        saveStateJson(finalStatePath);
    }

    return true;
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
