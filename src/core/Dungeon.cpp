#include "core/Dungeon.hpp"
#include <cmath>
#include <algorithm>

namespace Core {

Dungeon::Dungeon() {
    buildRooms();
}

void Dungeon::init() {
    buildRooms();
    m_currentRoomId = 1;
    m_transition.active = false;
}

void Dungeon::reset() {
    init();
}

void Dungeon::buildRooms() {
    m_rooms.clear();
    m_rooms.reserve(6);

    // Room 1: Entrance Foyer
    {
        DungeonRoom r1;
        r1.id = 1;
        r1.gridX = 1;
        r1.gridY = 2;
        r1.name = "Entrance Foyer";
        r1.hasPlate = true;
        r1.plate.position = {160.0f, 112.0f};
        r1.plate.pressed = false;
        r1.hasStairs = true;
        r1.stairsPos = {160.0f, 208.0f};

        // North door is closed initially until plate is pressed
        r1.setDoor(Direction::Up, DoorState::Closed);
        r1.setConnectedRoom(Direction::Up, 2);

        // South has stairs leading back outside, no room door
        r1.setDoor(Direction::Down, DoorState::None);
        r1.setDoor(Direction::Left, DoorState::None);
        r1.setDoor(Direction::Right, DoorState::None);

        m_rooms.push_back(r1);
    }

    // Room 2: Crystal Chasm
    {
        DungeonRoom r2;
        r2.id = 2;
        r2.gridX = 1;
        r2.gridY = 1;
        r2.name = "Crystal Chasm";

        // South door leads back to Room 1
        r2.setDoor(Direction::Down, DoorState::Open);
        r2.setConnectedRoom(Direction::Down, 1);

        // West door leads to Room 3 (Treasure Chamber)
        r2.setDoor(Direction::Left, DoorState::Open);
        r2.setConnectedRoom(Direction::Left, 3);

        // North door leads to Room 4 (Monster Arena)
        r2.setDoor(Direction::Up, DoorState::Open);
        r2.setConnectedRoom(Direction::Up, 4);

        // East door leads to Room 5 (Pre-Boss)
        r2.setDoor(Direction::Right, DoorState::Open);
        r2.setConnectedRoom(Direction::Right, 5);

        m_rooms.push_back(r2);
    }

    // Room 3: Treasure Chamber
    {
        DungeonRoom r3;
        r3.id = 3;
        r3.gridX = 0;
        r3.gridY = 1;
        r3.name = "Treasure Chamber";

        // East door leads back to Room 2
        r3.setDoor(Direction::Right, DoorState::Open);
        r3.setConnectedRoom(Direction::Right, 2);

        m_rooms.push_back(r3);
    }

    // Room 4: Monster Arena
    {
        DungeonRoom r4;
        r4.id = 4;
        r4.gridX = 1;
        r4.gridY = 0;
        r4.name = "Monster Arena";

        // South door leads back to Room 2
        r4.setDoor(Direction::Down, DoorState::Open);
        r4.setConnectedRoom(Direction::Down, 2);

        // East door leads to Room 5
        r4.setDoor(Direction::Right, DoorState::Open);
        r4.setConnectedRoom(Direction::Right, 5);

        m_rooms.push_back(r4);
    }

    // Room 5: Boss Gate Chamber
    {
        DungeonRoom r5;
        r5.id = 5;
        r5.gridX = 2;
        r5.gridY = 0;
        r5.name = "Boss Gate Chamber";

        // West door leads back to Room 4
        r5.setDoor(Direction::Left, DoorState::Open);
        r5.setConnectedRoom(Direction::Left, 4);

        // South door leads to Room 2
        r5.setDoor(Direction::Down, DoorState::Open);
        r5.setConnectedRoom(Direction::Down, 2);

        // North door leads to Boss Sanctum (Locked)
        r5.setDoor(Direction::Up, DoorState::Locked);
        r5.setConnectedRoom(Direction::Up, 6);

        m_rooms.push_back(r5);
    }

    // Room 6: Boss Sanctum
    {
        DungeonRoom r6;
        r6.id = 6;
        r6.gridX = 2;
        r6.gridY = -1;
        r6.name = "Boss Sanctum";

        // South door leads back to Room 5
        r6.setDoor(Direction::Down, DoorState::Open);
        r6.setConnectedRoom(Direction::Down, 5);

        m_rooms.push_back(r6);
    }
}

const DungeonRoom* Dungeon::getRoom(int id) const {
    for (const auto& r : m_rooms) {
        if (r.id == id) return &r;
    }
    return nullptr;
}

DungeonRoom* Dungeon::getRoom(int id) {
    for (auto& r : m_rooms) {
        if (r.id == id) return &r;
    }
    return nullptr;
}

const DungeonRoom& Dungeon::getCurrentRoom() const {
    const auto* r = getRoom(m_currentRoomId);
    if (r) return *r;
    return m_rooms.front();
}

DungeonRoom& Dungeon::getCurrentRoom() {
    auto* r = getRoom(m_currentRoomId);
    if (r) return *r;
    return m_rooms.front();
}

void Dungeon::setCurrentRoomId(int id) {
    if (getRoom(id)) {
        m_currentRoomId = id;
    }
}

bool Dungeon::checkCollision(const Rect& box) const {
    const auto& room = getCurrentRoom();

    // Wall thickness is 16 pixels
    constexpr float WALL_THICKNESS = 16.0f;
    constexpr float DOOR_START_X = 136.0f;
    constexpr float DOOR_END_X = 184.0f;
    constexpr float DOOR_START_Y = 96.0f;
    constexpr float DOOR_END_Y = 144.0f;

    // Check North Wall
    if (box.top() < WALL_THICKNESS) {
        bool inDoorway = (box.left() >= DOOR_START_X && box.right() <= DOOR_END_X);
        if (!inDoorway || room.getDoor(Direction::Up) != DoorState::Open) {
            return true;
        }
    }

    // Check South Wall
    if (box.bottom() > DungeonRoom::ROOM_HEIGHT - WALL_THICKNESS) {
        bool inDoorway = (box.left() >= DOOR_START_X && box.right() <= DOOR_END_X);
        if (!inDoorway || room.getDoor(Direction::Down) != DoorState::Open) {
            return true;
        }
    }

    // Check West Wall
    if (box.left() < WALL_THICKNESS) {
        bool inDoorway = (box.top() >= DOOR_START_Y && box.bottom() <= DOOR_END_Y);
        if (!inDoorway || room.getDoor(Direction::Left) != DoorState::Open) {
            return true;
        }
    }

    // Check East Wall
    if (box.right() > DungeonRoom::ROOM_WIDTH - WALL_THICKNESS) {
        bool inDoorway = (box.top() >= DOOR_START_Y && box.bottom() <= DOOR_END_Y);
        if (!inDoorway || room.getDoor(Direction::Right) != DoorState::Open) {
            return true;
        }
    }

    return false;
}

Vec2 Dungeon::resolveMovement(const Vec2& currentPos, const Vec2& displacement, const Rect& localHitbox) const {
    Vec2 newPos = currentPos;

    // 1. Try X axis movement
    if (displacement.x != 0.0f) {
        Vec2 testPos = { newPos.x + displacement.x, newPos.y };
        Rect testBox(testPos.x + localHitbox.x, testPos.y + localHitbox.y, localHitbox.width, localHitbox.height);
        if (!checkCollision(testBox)) {
            newPos.x = testPos.x;
        }
    }

    // 2. Try Y axis movement
    if (displacement.y != 0.0f) {
        Vec2 testPos = { newPos.x, newPos.y + displacement.y };
        Rect testBox(testPos.x + localHitbox.x, testPos.y + localHitbox.y, localHitbox.width, localHitbox.height);
        if (!checkCollision(testBox)) {
            newPos.y = testPos.y;
        }
    }

    return newPos;
}

bool Dungeon::checkPlateTrigger(const Rect& playerHitbox) {
    auto& room = getCurrentRoom();
    if (!room.hasPlate) return false;

    if (room.plate.checkTrigger(playerHitbox)) {
        // Unlock connected door (Room 1 unlocks North door)
        if (room.id == 1) {
            room.setDoor(Direction::Up, DoorState::Open);
        }
        return true;
    }
    return false;
}

bool Dungeon::checkDoorwayTransition(const Rect& playerHitbox, Direction facingDir, const Vec2& playerPos) {
    if (m_transition.active) return false;

    const auto& room = getCurrentRoom();

    constexpr float DOOR_MIN_X = 136.0f;
    constexpr float DOOR_MAX_X = 184.0f;
    constexpr float DOOR_MIN_Y = 96.0f;
    constexpr float DOOR_MAX_Y = 144.0f;

    // North doorway
    if (facingDir == Direction::Up && playerPos.y <= 18.0f &&
        playerHitbox.right() >= DOOR_MIN_X && playerHitbox.left() <= DOOR_MAX_X) {
        if (room.getDoor(Direction::Up) == DoorState::Open) {
            int nextRoom = room.getConnectedRoom(Direction::Up);
            if (nextRoom != -1) {
                startTransition(Direction::Up, nextRoom, playerPos);
                return true;
            }
        }
    }

    // South doorway
    if (facingDir == Direction::Down && playerPos.y >= DungeonRoom::ROOM_HEIGHT - 26.0f &&
        playerHitbox.right() >= DOOR_MIN_X && playerHitbox.left() <= DOOR_MAX_X) {
        if (room.getDoor(Direction::Down) == DoorState::Open) {
            int nextRoom = room.getConnectedRoom(Direction::Down);
            if (nextRoom != -1) {
                startTransition(Direction::Down, nextRoom, playerPos);
                return true;
            }
        }
    }

    // West doorway
    if (facingDir == Direction::Left && playerPos.x <= 18.0f &&
        playerHitbox.bottom() >= DOOR_MIN_Y && playerHitbox.top() <= DOOR_MAX_Y) {
        if (room.getDoor(Direction::Left) == DoorState::Open) {
            int nextRoom = room.getConnectedRoom(Direction::Left);
            if (nextRoom != -1) {
                startTransition(Direction::Left, nextRoom, playerPos);
                return true;
            }
        }
    }

    // East doorway
    if (facingDir == Direction::Right && playerPos.x >= DungeonRoom::ROOM_WIDTH - 26.0f &&
        playerHitbox.bottom() >= DOOR_MIN_Y && playerHitbox.top() <= DOOR_MAX_Y) {
        if (room.getDoor(Direction::Right) == DoorState::Open) {
            int nextRoom = room.getConnectedRoom(Direction::Right);
            if (nextRoom != -1) {
                startTransition(Direction::Right, nextRoom, playerPos);
                return true;
            }
        }
    }

    return false;
}

bool Dungeon::checkStairsExit(const Rect& playerHitbox) const {
    const auto& room = getCurrentRoom();
    if (room.hasStairs) {
        return room.getStairsBounds().overlaps(playerHitbox);
    }
    return false;
}

Vec2 Dungeon::getRoomCameraCenter(int gridX, int gridY) {
    return Vec2{gridX * DungeonRoom::ROOM_WIDTH, gridY * DungeonRoom::ROOM_HEIGHT};
}

Vec2 Dungeon::getTargetLocalPlayerPos() const {
    switch (m_transition.direction) {
        case Direction::Up:
            return Vec2{152.0f, DungeonRoom::ROOM_HEIGHT - 36.0f};
        case Direction::Down:
            return Vec2{152.0f, 28.0f};
        case Direction::Left:
            return Vec2{DungeonRoom::ROOM_WIDTH - 36.0f, 116.0f};
        case Direction::Right:
            return Vec2{28.0f, 116.0f};
    }
    return Vec2{152.0f, 116.0f};
}

void Dungeon::startTransition(Direction dir, int targetRoomId, const Vec2& currentPos) {
    const auto* fromRoom = getRoom(m_currentRoomId);
    const auto* toRoom = getRoom(targetRoomId);
    if (!fromRoom || !toRoom) return;

    m_transition.active = true;
    m_transition.direction = dir;
    m_transition.fromRoomId = fromRoom->id;
    m_transition.toRoomId = toRoom->id;
    m_transition.timer = 0.0f;
    m_transition.duration = 0.5f;

    m_transition.startCameraPos = getRoomCameraCenter(fromRoom->gridX, fromRoom->gridY);
    m_transition.targetCameraPos = getRoomCameraCenter(toRoom->gridX, toRoom->gridY);

    m_transition.startPlayerPos = m_transition.startCameraPos + currentPos;
    m_transition.targetPlayerPos = m_transition.targetCameraPos + getTargetLocalPlayerPos();
}

void Dungeon::update(float dt) {
    if (!m_transition.active) return;

    m_transition.timer += dt;
    if (m_transition.timer >= m_transition.duration) {
        m_transition.active = false;
        m_currentRoomId = m_transition.toRoomId;
    }
}

Vec2 Dungeon::getCameraOffset() const {
    if (!m_transition.active) {
        const auto& room = getCurrentRoom();
        return getRoomCameraCenter(room.gridX, room.gridY);
    }

    float t = m_transition.progress();
    // Smooth cosine interpolation
    float smoothT = 0.5f * (1.0f - std::cos(t * 3.14159265f));

    return Vec2{
        m_transition.startCameraPos.x + (m_transition.targetCameraPos.x - m_transition.startCameraPos.x) * smoothT,
        m_transition.startCameraPos.y + (m_transition.targetCameraPos.y - m_transition.startCameraPos.y) * smoothT
    };
}

Vec2 Dungeon::getTransitionPlayerPos() const {
    if (!m_transition.active) {
        return Vec2{160.0f, 120.0f};
    }

    float t = m_transition.progress();
    return Vec2{
        m_transition.startPlayerPos.x + (m_transition.targetPlayerPos.x - m_transition.startPlayerPos.x) * t,
        m_transition.startPlayerPos.y + (m_transition.targetPlayerPos.y - m_transition.startPlayerPos.y) * t
    };
}

} // namespace Core
