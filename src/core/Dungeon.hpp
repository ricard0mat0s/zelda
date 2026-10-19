#pragma once

#include "core/types.hpp"
#include <array>
#include <string>
#include <vector>

namespace Core {

enum class DungeonTileType {
    Floor = 0,
    Wall,
    DoorClosed,
    DoorOpen,
    Stairs,
    Chasm
};

enum class DoorState {
    None = 0,
    Closed,
    Open,
    Locked
};

struct PressurePlate {
    Vec2 position{160.0f, 112.0f}; // Center of room
    bool pressed = false;

    Rect getBounds() const {
        return Rect{position.x - 7.0f, position.y - 7.0f, 14.0f, 14.0f};
    }

    bool checkTrigger(const Rect& playerHitbox) {
        if (!pressed && getBounds().overlaps(playerHitbox)) {
            pressed = true;
            return true;
        }
        return false;
    }
};

struct CrystalSwitch {
    Vec2 position{160.0f, 64.0f};
    bool active = false;

    Rect getHitbox() const {
        return Rect{position.x - 7.0f, position.y - 7.0f, 14.0f, 14.0f};
    }

    bool checkHit(const Rect& arrowHitbox) {
        if (!active && getHitbox().overlaps(arrowHitbox)) {
            active = true;
            return true;
        }
        return false;
    }
};

struct TreasureChest {
    Vec2 position{160.0f, 96.0f};
    bool opened = false;

    Rect getHitbox() const {
        return Rect{position.x - 8.0f, position.y - 8.0f, 16.0f, 16.0f};
    }

    bool canOpen(const Vec2& playerPos, Direction facing) const {
        if (opened) return false;
        float dx = playerPos.x - position.x;
        float dy = playerPos.y - position.y;
        float distSq = dx * dx + dy * dy;
        if (distSq > 32.0f * 32.0f) return false;

        if (facing == Direction::Up && playerPos.y >= position.y) return true;
        if (facing == Direction::Down && playerPos.y <= position.y) return true;
        if (facing == Direction::Left && playerPos.x >= position.x) return true;
        if (facing == Direction::Right && playerPos.x <= position.x) return true;

        return distSq <= 20.0f * 20.0f;
    }

    bool open() {
        if (!opened) {
            opened = true;
            return true;
        }
        return false;
    }
};

struct DungeonRoom {
    int id = 1;
    int gridX = 0;
    int gridY = 0;
    std::string name;

    static constexpr int COLS = 20;
    static constexpr int ROWS = 15;
    static constexpr int TILE_SIZE = 16;
    static constexpr float ROOM_WIDTH = 320.0f;
    static constexpr float ROOM_HEIGHT = 240.0f;

    // Doors: 0: Down (South), 1: Up (North), 2: Left (West), 3: Right (East)
    std::array<DoorState, 4> doors = {DoorState::None, DoorState::None, DoorState::None, DoorState::None};
    
    // Connected room IDs for each direction (-1 if none)
    std::array<int, 4> connectedRooms = {-1, -1, -1, -1};

    PressurePlate plate;
    bool hasPlate = false;
    bool hasStairs = false;
    Vec2 stairsPos{160.0f, 208.0f};

    CrystalSwitch crystalSwitch;
    bool hasCrystalSwitch = false;

    TreasureChest chest;
    bool hasChest = false;

    std::vector<Rect> chasmPits;

    DoorState getDoor(Direction dir) const {
        return doors[static_cast<size_t>(dir)];
    }

    void setDoor(Direction dir, DoorState state) {
        doors[static_cast<size_t>(dir)] = state;
    }

    int getConnectedRoom(Direction dir) const {
        return connectedRooms[static_cast<size_t>(dir)];
    }

    void setConnectedRoom(Direction dir, int roomId) {
        connectedRooms[static_cast<size_t>(dir)] = roomId;
    }

    Rect getStairsBounds() const {
        return Rect{stairsPos.x - 8.0f, stairsPos.y - 8.0f, 16.0f, 16.0f};
    }
};

struct RoomTransition {
    bool active = false;
    Direction direction = Direction::Up;
    int fromRoomId = 1;
    int toRoomId = 2;
    float timer = 0.0f;
    float duration = 0.5f;

    Vec2 startCameraPos{0.0f, 0.0f};
    Vec2 targetCameraPos{0.0f, 0.0f};
    Vec2 startPlayerPos{0.0f, 0.0f};
    Vec2 targetPlayerPos{0.0f, 0.0f};

    float progress() const {
        if (duration <= 0.0f) return 1.0f;
        float t = timer / duration;
        return t > 1.0f ? 1.0f : (t < 0.0f ? 0.0f : t);
    }
};

class Dungeon {
public:
    Dungeon();

    void init();
    void reset();

    void update(float dt);

    int getCurrentRoomId() const { return m_currentRoomId; }
    void setCurrentRoomId(int id);

    const DungeonRoom* getRoom(int id) const;
    DungeonRoom* getRoom(int id);

    const DungeonRoom& getCurrentRoom() const;
    DungeonRoom& getCurrentRoom();

    bool checkCollision(const Rect& box) const;
    Vec2 resolveMovement(const Vec2& currentPos, const Vec2& displacement, const Rect& localHitbox) const;

    // Checks pressure plate in current room; returns true if triggered for first time
    bool checkPlateTrigger(const Rect& playerHitbox);

    // Checks doorway threshold; starts transition if valid and returns true
    bool checkDoorwayTransition(const Rect& playerHitbox, Direction facingDir, const Vec2& playerPos);

    // Checks stairs in Room 1 leading to overworld; returns true if stepped on
    bool checkStairsExit(const Rect& playerHitbox) const;

    bool checkProjectileCollision(const Rect& arrowHitbox) const;

    // Checks crystal switch hit by projectile; returns true if triggered for first time
    bool checkCrystalSwitchHit(const Rect& arrowHitbox);

    // Checks if player can open treasure chest in current room; returns true if opened
    bool checkChestInteract(const Vec2& playerPos, Direction facing);

    const RoomTransition& getTransition() const { return m_transition; }
    bool isTransitioning() const { return m_transition.active; }

    Vec2 getCameraOffset() const;
    Vec2 getTransitionPlayerPos() const;
    Vec2 getTargetLocalPlayerPos() const;

    static Vec2 getRoomCameraCenter(int gridX, int gridY);

private:
    void buildRooms();
    void startTransition(Direction dir, int targetRoomId, const Vec2& currentPos);

    std::vector<DungeonRoom> m_rooms;
    int m_currentRoomId = 1;
    RoomTransition m_transition;
};

} // namespace Core
