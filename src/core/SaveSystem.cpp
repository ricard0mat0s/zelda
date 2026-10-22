#include "core/SaveSystem.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cctype>
#include <algorithm>

namespace Core {

namespace {

std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }
    return s.substr(start, end - start);
}

bool extractRawValue(const std::string& json, const std::string& key, std::string& outVal) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = json.find(pattern);
    if (pos == std::string::npos) return false;

    size_t colon = json.find(':', pos + pattern.size());
    if (colon == std::string::npos) return false;

    size_t valStart = colon + 1;
    while (valStart < json.size() && std::isspace(static_cast<unsigned char>(json[valStart]))) {
        valStart++;
    }
    if (valStart >= json.size()) return false;

    size_t valEnd = valStart;
    while (valEnd < json.size() && json[valEnd] != ',' && json[valEnd] != '}' &&
           !std::isspace(static_cast<unsigned char>(json[valEnd]))) {
        valEnd++;
    }

    outVal = json.substr(valStart, valEnd - valStart);
    return !outVal.empty();
}

bool extractInt(const std::string& json, const std::string& key, int& outInt) {
    std::string raw;
    if (!extractRawValue(json, key, raw)) return false;
    try {
        size_t idx = 0;
        int v = std::stoi(raw, &idx);
        if (idx != raw.size()) return false;
        outInt = v;
        return true;
    } catch (...) {
        return false;
    }
}

bool extractFloat(const std::string& json, const std::string& key, float& outFloat) {
    std::string raw;
    if (!extractRawValue(json, key, raw)) return false;
    try {
        size_t idx = 0;
        float v = std::stof(raw, &idx);
        if (idx != raw.size()) return false;
        outFloat = v;
        return true;
    } catch (...) {
        return false;
    }
}

bool extractBool(const std::string& json, const std::string& key, bool& outBool) {
    std::string raw;
    if (!extractRawValue(json, key, raw)) return false;
    if (raw == "true") {
        outBool = true;
        return true;
    }
    if (raw == "false") {
        outBool = false;
        return true;
    }
    return false;
}

} // namespace

std::string SaveSystem::serialize(const SaveData& data) {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"version\": " << data.version << ",\n";
    ss << "  \"maxHearts\": " << data.maxHearts << ",\n";
    ss << "  \"hasSword\": " << (data.hasSword ? "true" : "false") << ",\n";
    ss << "  \"hasBow\": " << (data.hasBow ? "true" : "false") << ",\n";
    ss << "  \"keys\": " << data.keys << ",\n";
    ss << "  \"inDungeon\": " << (data.inDungeon ? "true" : "false") << ",\n";
    ss << "  \"dungeonRoomId\": " << data.dungeonRoomId << ",\n";
    ss << "  \"respawnX\": " << data.respawnPos.x << ",\n";
    ss << "  \"respawnY\": " << data.respawnPos.y << ",\n";
    ss << "  \"respawnFacing\": " << static_cast<int>(data.respawnFacing) << ",\n";
    ss << "  \"room1PlatePressed\": " << (data.room1PlatePressed ? "true" : "false") << ",\n";
    ss << "  \"room2SwitchActive\": " << (data.room2SwitchActive ? "true" : "false") << ",\n";
    ss << "  \"room3ChestOpened\": " << (data.room3ChestOpened ? "true" : "false") << ",\n";
    ss << "  \"room4ArenaCleared\": " << (data.room4ArenaCleared ? "true" : "false") << ",\n";
    ss << "  \"room5BossGateOpen\": " << (data.room5BossGateOpen ? "true" : "false") << ",\n";
    ss << "  \"room6BossDefeated\": " << (data.room6BossDefeated ? "true" : "false") << "\n";
    ss << "}\n";
    return ss.str();
}

bool SaveSystem::deserialize(const std::string& jsonStr, SaveData& outData) {
    outData = SaveData{}; // Reset to safe fallback defaults first

    std::string trimmed = trim(jsonStr);
    if (trimmed.size() < 2 || trimmed.front() != '{' || trimmed.back() != '}') {
        return false;
    }

    SaveData parsed;
    if (!extractInt(trimmed, "version", parsed.version) || parsed.version != 1) {
        return false;
    }
    if (!extractInt(trimmed, "maxHearts", parsed.maxHearts) || parsed.maxHearts < 1 || parsed.maxHearts > 20) {
        return false;
    }
    if (!extractBool(trimmed, "hasSword", parsed.hasSword)) return false;
    if (!extractBool(trimmed, "hasBow", parsed.hasBow)) return false;
    if (!extractInt(trimmed, "keys", parsed.keys) || parsed.keys < 0 || parsed.keys > 99) return false;
    if (!extractBool(trimmed, "inDungeon", parsed.inDungeon)) return false;
    if (!extractInt(trimmed, "dungeonRoomId", parsed.dungeonRoomId) || parsed.dungeonRoomId < 1 || parsed.dungeonRoomId > 6) {
        return false;
    }

    if (!extractFloat(trimmed, "respawnX", parsed.respawnPos.x)) return false;
    if (!extractFloat(trimmed, "respawnY", parsed.respawnPos.y)) return false;

    int facingInt = 0;
    if (!extractInt(trimmed, "respawnFacing", facingInt) || facingInt < 0 || facingInt > 3) {
        return false;
    }
    parsed.respawnFacing = static_cast<Direction>(facingInt);

    if (!extractBool(trimmed, "room1PlatePressed", parsed.room1PlatePressed)) return false;
    if (!extractBool(trimmed, "room2SwitchActive", parsed.room2SwitchActive)) return false;
    if (!extractBool(trimmed, "room3ChestOpened", parsed.room3ChestOpened)) return false;
    if (!extractBool(trimmed, "room4ArenaCleared", parsed.room4ArenaCleared)) return false;
    if (!extractBool(trimmed, "room5BossGateOpen", parsed.room5BossGateOpen)) return false;
    if (!extractBool(trimmed, "room6BossDefeated", parsed.room6BossDefeated)) return false;

    outData = parsed;
    return true;
}

bool SaveSystem::saveToFile(const SaveData& data, const std::string& filePath) {
    std::ofstream ofs(filePath, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << serialize(data);
    return ofs.good();
}

bool SaveSystem::loadFromFile(SaveData& outData, const std::string& filePath) {
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        outData = SaveData{};
        return false;
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return deserialize(ss.str(), outData);
}

bool SaveSystem::hasSaveFile(const std::string& filePath) {
    std::error_code ec;
    return std::filesystem::exists(filePath, ec);
}

bool SaveSystem::deleteSaveFile(const std::string& filePath) {
    std::error_code ec;
    return std::filesystem::remove(filePath, ec);
}

SaveData SaveSystem::captureState(const Player& player, const Dungeon& dungeon,
                                  bool inDungeon, const Vec2& entrancePos, Direction entranceFacing,
                                  bool bossDefeated) {
    SaveData data;
    data.version = 1;
    data.maxHearts = player.getHealth().getMaxHearts();
    data.hasSword = player.getInventory().hasSword;
    data.hasBow = player.getInventory().hasBow;
    data.keys = player.getInventory().keys;

    data.inDungeon = inDungeon;
    data.dungeonRoomId = dungeon.getCurrentRoomId();
    data.respawnPos = entrancePos;
    data.respawnFacing = entranceFacing;

    if (const auto* r1 = dungeon.getRoom(1)) {
        data.room1PlatePressed = r1->plate.pressed;
    }
    if (const auto* r2 = dungeon.getRoom(2)) {
        data.room2SwitchActive = r2->crystalSwitch.active;
    }
    if (const auto* r3 = dungeon.getRoom(3)) {
        data.room3ChestOpened = r3->chest.opened;
    }
    if (const auto* r4 = dungeon.getRoom(4)) {
        data.room4ArenaCleared = r4->arenaCleared;
    }
    if (const auto* r5 = dungeon.getRoom(5)) {
        data.room5BossGateOpen = (r5->getDoor(Direction::Up) == DoorState::Open);
    }
    data.room6BossDefeated = bossDefeated;

    return data;
}

void SaveSystem::applyState(const SaveData& data, Player& player, Dungeon& dungeon) {
    player.setHealth(Health(data.maxHearts));
    player.getInventory().hasSword = data.hasSword;
    player.getInventory().hasBow = data.hasBow;
    player.getInventory().keys = data.keys;
    player.setPosition(data.respawnPos);
    player.setFacing(data.respawnFacing);

    dungeon.setCurrentRoomId(data.dungeonRoomId);

    if (auto* r1 = dungeon.getRoom(1)) {
        r1->plate.pressed = data.room1PlatePressed;
        if (data.room1PlatePressed) {
            r1->setDoor(Direction::Up, DoorState::Open);
        }
    }
    if (auto* r2 = dungeon.getRoom(2)) {
        r2->crystalSwitch.active = data.room2SwitchActive;
        if (data.room2SwitchActive) {
            r2->setDoor(Direction::Up, DoorState::Open);
        }
    }
    if (auto* r3 = dungeon.getRoom(3)) {
        r3->chest.opened = data.room3ChestOpened;
    }
    if (auto* r4 = dungeon.getRoom(4)) {
        r4->arenaCleared = data.room4ArenaCleared;
        r4->arenaLocked = false;
        r4->setDoor(Direction::Down, DoorState::Open);
        r4->setDoor(Direction::Right, DoorState::Open);
    }
    if (auto* r5 = dungeon.getRoom(5)) {
        if (data.room5BossGateOpen) {
            r5->setDoor(Direction::Up, DoorState::Open);
        }
    }
    if (auto* r6 = dungeon.getRoom(6)) {
        r6->arenaCleared = data.room6BossDefeated;
        r6->arenaLocked = false;
        r6->setDoor(Direction::Down, DoorState::Open);
    }
}

} // namespace Core
