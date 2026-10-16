#pragma once

namespace Core {

struct GameConfig {
    static constexpr int VIRTUAL_WIDTH = 320;
    static constexpr int VIRTUAL_HEIGHT = 240;
    static constexpr int DEFAULT_SCALE = 3; // 960 x 720 window
    static constexpr int TARGET_FPS = 60;

    static constexpr int TILE_SIZE = 16;
    static constexpr int MAP_COLS = 40;
    static constexpr int MAP_ROWS = 30;
    static constexpr int WORLD_WIDTH = MAP_COLS * TILE_SIZE;   // 640
    static constexpr int WORLD_HEIGHT = MAP_ROWS * TILE_SIZE; // 480

    static constexpr float PLAYER_SPEED = 80.0f; // pixels per second
    static constexpr int INITIAL_MAX_HEARTS = 3;
    static constexpr int HALF_HEARTS_PER_CONTAINER = 2;
    static constexpr int INITIAL_HEALTH = INITIAL_MAX_HEARTS * HALF_HEARTS_PER_CONTAINER; // 6
    static constexpr float HURT_INVULNERABILITY_SECS = 1.0f;
    static constexpr float SWORD_ATTACK_DURATION_SECS = 0.25f;
};

} // namespace Core
