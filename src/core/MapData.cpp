#include "core/MapData.hpp"
#include <algorithm>
#include <cmath>

namespace Core {

MapData::MapData()
    : m_ground(COLS * ROWS, GroundTile::Grass),
      m_obstacles(COLS * ROWS, ObstacleTile::None) {
    buildOverworldMap();
}

void MapData::buildOverworldMap() {
    std::fill(m_ground.begin(), m_ground.end(), GroundTile::Grass);
    std::fill(m_obstacles.begin(), m_obstacles.end(), ObstacleTile::None);

    // 1. Pathways connecting areas
    // Central path going north
    for (int y = 8; y <= 24; ++y) {
        setGround(12, y, GroundTile::Path);
        setGround(13, y, GroundTile::Path);
    }
    // Horizontal cross path
    for (int x = 6; x <= 32; ++x) {
        setGround(x, 17, GroundTile::Path);
    }
    // Northern trail leading to ruins
    for (int y = 3; y <= 8; ++y) {
        setGround(20, y, GroundTile::Path);
    }

    // 2. Obstacles (Trees, Water, Walls)
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            // Perimeter barrier trees (2-tile thick border)
            bool isPerimeter = (x <= 1 || x >= COLS - 2 || y <= 1 || y >= ROWS - 2);
            if (isPerimeter) {
                setObstacle(x, y, ObstacleTile::Tree);
                continue;
            }

            // Northeastern Water Pond (x: 26..33, y: 4..9)
            bool isPond = (x >= 26 && x <= 33 && y >= 4 && y <= 9);
            if (isPond) {
                setObstacle(x, y, ObstacleTile::Water);
                continue;
            }

            // Northern Stone Ruins (x: 17..23, y: 2..5)
            bool isRuinsWall = (y == 2 && x >= 17 && x <= 23) ||
                               (x == 17 && y >= 3 && y <= 4) ||
                               (x == 23 && y >= 3 && y <= 4);
            if (isRuinsWall) {
                setObstacle(x, y, ObstacleTile::StoneWall);
                continue;
            }

            // Forest clusters
            bool isCluster1 = (x >= 4 && x <= 8 && y >= 4 && y <= 7);
            bool isCluster2 = (x >= 5 && x <= 9 && y >= 21 && y <= 25);
            bool isCluster3 = (x >= 24 && x <= 28 && y >= 20 && y <= 24);

            if (isCluster1 || isCluster2 || isCluster3) {
                setObstacle(x, y, ObstacleTile::Tree);
            }
        }
    }
}

GroundTile MapData::getGround(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return GroundTile::Grass;
    return m_ground[row * COLS + col];
}

ObstacleTile MapData::getObstacle(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return ObstacleTile::Tree; // Out of bounds is solid
    return m_obstacles[row * COLS + col];
}

void MapData::setGround(int col, int row, GroundTile tile) {
    if (col >= 0 && col < COLS && row >= 0 && row < ROWS) {
        m_ground[row * COLS + col] = tile;
    }
}

void MapData::setObstacle(int col, int row, ObstacleTile tile) {
    if (col >= 0 && col < COLS && row >= 0 && row < ROWS) {
        m_obstacles[row * COLS + col] = tile;
    }
}

bool MapData::isSolid(int col, int row) const {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return true;
    return m_obstacles[row * COLS + col] != ObstacleTile::None;
}

bool MapData::checkRectCollision(const Rect& box) const {
    // Check bounds
    if (box.left() < 0.0f || box.right() > GameConfig::WORLD_WIDTH ||
        box.top() < 0.0f || box.bottom() > GameConfig::WORLD_HEIGHT) {
        return true;
    }

    int startCol = std::max(0, static_cast<int>(std::floor(box.left() / TILE_SIZE)));
    int endCol = std::min(COLS - 1, static_cast<int>(std::floor(box.right() / TILE_SIZE)));
    int startRow = std::max(0, static_cast<int>(std::floor(box.top() / TILE_SIZE)));
    int endRow = std::min(ROWS - 1, static_cast<int>(std::floor(box.bottom() / TILE_SIZE)));

    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            if (isSolid(c, r)) {
                Rect tileBox(static_cast<float>(c * TILE_SIZE), static_cast<float>(r * TILE_SIZE),
                             static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE));
                if (box.overlaps(tileBox)) {
                    return true;
                }
            }
        }
    }
    return false;
}

Vec2 MapData::resolveMovement(const Vec2& currentPos, const Vec2& displacement, const Rect& localHitbox) const {
    Vec2 newPos = currentPos;

    // 1. Try X axis movement
    if (displacement.x != 0.0f) {
        Vec2 testPos = { newPos.x + displacement.x, newPos.y };
        Rect testBox(testPos.x + localHitbox.x, testPos.y + localHitbox.y, localHitbox.width, localHitbox.height);
        if (!checkRectCollision(testBox)) {
            newPos.x = testPos.x;
        }
    }

    // 2. Try Y axis movement
    if (displacement.y != 0.0f) {
        Vec2 testPos = { newPos.x, newPos.y + displacement.y };
        Rect testBox(testPos.x + localHitbox.x, testPos.y + localHitbox.y, localHitbox.width, localHitbox.height);
        if (!checkRectCollision(testBox)) {
            newPos.y = testPos.y;
        }
    }

    // Clamp inside world boundaries
    const float minX = -localHitbox.x;
    const float maxX = GameConfig::WORLD_WIDTH - localHitbox.x - localHitbox.width;
    const float minY = -localHitbox.y;
    const float maxY = GameConfig::WORLD_HEIGHT - localHitbox.y - localHitbox.height;

    newPos.x = std::clamp(newPos.x, minX, maxX);
    newPos.y = std::clamp(newPos.y, minY, maxY);

    return newPos;
}

} // namespace Core
