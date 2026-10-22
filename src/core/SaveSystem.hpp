#pragma once

#include "core/types.hpp"
#include "core/Player.hpp"
#include "core/Dungeon.hpp"
#include <string>

namespace Core {

struct SaveData {
    int version = 1;
    int maxHearts = 3;
    bool hasSword = false;
    bool hasBow = false;
    int keys = 0;

    bool inDungeon = false;
    int dungeonRoomId = 1;
    Vec2 respawnPos{200.0f, 280.0f};
    Direction respawnFacing = Direction::Down;

    bool room1PlatePressed = false;
    bool room2SwitchActive = false;
    bool room3ChestOpened = false;
    bool room4ArenaCleared = false;
    bool room5BossGateOpen = false;
    bool room6BossDefeated = false;
};

class SaveSystem {
public:
    static constexpr const char* DEFAULT_SAVE_PATH = "savegame.json";

    // Serializes SaveData to a formatted JSON string
    static std::string serialize(const SaveData& data);

    // Deserializes SaveData from a JSON string; returns false and resets to defaults if corrupted
    static bool deserialize(const std::string& jsonStr, SaveData& outData);

    // File I/O persistence
    static bool saveToFile(const SaveData& data, const std::string& filePath = DEFAULT_SAVE_PATH);
    static bool loadFromFile(SaveData& outData, const std::string& filePath = DEFAULT_SAVE_PATH);
    static bool hasSaveFile(const std::string& filePath = DEFAULT_SAVE_PATH);
    static bool deleteSaveFile(const std::string& filePath = DEFAULT_SAVE_PATH);

    // Captures live state from Player + Dungeon into SaveData
    static SaveData captureState(const Player& player, const Dungeon& dungeon,
                                 bool inDungeon, const Vec2& entrancePos, Direction entranceFacing,
                                 bool bossDefeated);

    // Applies SaveData onto Player + Dungeon
    static void applyState(const SaveData& data, Player& player, Dungeon& dungeon);
};

} // namespace Core
