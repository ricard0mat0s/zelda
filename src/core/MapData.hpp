#pragma once

#include "core/types.hpp"
#include "core/GameConfig.hpp"
#include <vector>
#include <cstdint>

namespace Core {

enum class GroundTile : uint8_t {
    Grass = 0,
    Path
};

enum class ObstacleTile : uint8_t {
    None = 0,
    Tree,
    Water,
    StoneWall
};

class MapData {
public:
    static constexpr int COLS = GameConfig::MAP_COLS;
    static constexpr int ROWS = GameConfig::MAP_ROWS;
    static constexpr int TILE_SIZE = GameConfig::TILE_SIZE;

    MapData();

    void buildOverworldMap();

    GroundTile getGround(int col, int row) const;
    ObstacleTile getObstacle(int col, int row) const;

    void setGround(int col, int row, GroundTile tile);
    void setObstacle(int col, int row, ObstacleTile tile);

    bool isSolid(int col, int row) const;
    bool checkRectCollision(const Rect& box) const;

    // Resolves movement with sliding against solid tiles and world boundary clamping
    Vec2 resolveMovement(const Vec2& currentPos, const Vec2& displacement, const Rect& localHitbox) const;

private:
    std::vector<GroundTile> m_ground;
    std::vector<ObstacleTile> m_obstacles;
};

} // namespace Core
