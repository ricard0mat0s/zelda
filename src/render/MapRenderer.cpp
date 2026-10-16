#include "render/MapRenderer.hpp"
#include "render/AssetManager.hpp"
#include <algorithm>
#include <cmath>

namespace Render {

void MapRenderer::render(const Core::MapData& map, const Camera2D& camera) {
    auto& assets = AssetManager::instance();
    Texture2D grassTex = assets.getTexture("grass");
    Texture2D pathTex = assets.getTexture("path");
    Texture2D treeTex = assets.getTexture("tree");
    Texture2D waterTex = assets.getTexture("water");
    Texture2D stoneWallTex = assets.getTexture("stone_wall");

    constexpr int tileSize = Core::MapData::TILE_SIZE;

    // Compute visible tile range based on camera target & offset
    float viewX = camera.target.x - camera.offset.x;
    float viewY = camera.target.y - camera.offset.y;

    int startCol = std::max(0, static_cast<int>(std::floor(viewX / tileSize)));
    int endCol = std::min(Core::MapData::COLS - 1, static_cast<int>(std::ceil((viewX + Core::GameConfig::VIRTUAL_WIDTH) / tileSize)));
    int startRow = std::max(0, static_cast<int>(std::floor(viewY / tileSize)));
    int endRow = std::min(Core::MapData::ROWS - 1, static_cast<int>(std::ceil((viewY + Core::GameConfig::VIRTUAL_HEIGHT) / tileSize)));

    // 1. Ground Layer Pass
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            float posX = static_cast<float>(c * tileSize);
            float posY = static_cast<float>(r * tileSize);

            Core::GroundTile g = map.getGround(c, r);
            if (g == Core::GroundTile::Path) {
                DrawTexture(pathTex, static_cast<int>(posX), static_cast<int>(posY), WHITE);
            } else {
                DrawTexture(grassTex, static_cast<int>(posX), static_cast<int>(posY), WHITE);
            }
        }
    }

    // 2. Obstacle Layer Pass
    for (int r = startRow; r <= endRow; ++r) {
        for (int c = startCol; c <= endCol; ++c) {
            float posX = static_cast<float>(c * tileSize);
            float posY = static_cast<float>(r * tileSize);

            Core::ObstacleTile obs = map.getObstacle(c, r);
            switch (obs) {
                case Core::ObstacleTile::Tree:
                    DrawTexture(treeTex, static_cast<int>(posX), static_cast<int>(posY), WHITE);
                    break;
                case Core::ObstacleTile::Water:
                    DrawTexture(waterTex, static_cast<int>(posX), static_cast<int>(posY), WHITE);
                    break;
                case Core::ObstacleTile::StoneWall:
                    DrawTexture(stoneWallTex, static_cast<int>(posX), static_cast<int>(posY), WHITE);
                    break;
                case Core::ObstacleTile::None:
                default:
                    break;
            }
        }
    }
}

} // namespace Render
