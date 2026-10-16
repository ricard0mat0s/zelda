#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../src/core/types.hpp"
#include "../src/core/GameConfig.hpp"
#include "../src/core/SoundGenerator.hpp"
#include "../src/core/Movement.hpp"
#include "../src/core/MapData.hpp"
#include "../src/core/Player.hpp"
#include "../src/render/GeneratedAssets.hpp"

using namespace Core;

static int testsPassed = 0;
static int testsFailed = 0;

#define TEST_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            std::cerr << "FAILED: " << msg << " (" << #expr << ") at " << __FILE__ << ":" << __LINE__ << std::endl; \
            testsFailed++; \
            return; \
        } \
    } while(0)

void test_vector_math() {
    Vec2 a(3.0f, 4.0f);
    TEST_ASSERT(a.length() == 5.0f, "Vector length should be 5 for (3, 4)");

    Vec2 b(1.0f, 2.0f);
    Vec2 c = a + b;
    TEST_ASSERT(c.x == 4.0f && c.y == 6.0f, "Vector addition failed");

    Vec2 norm = a.normalized();
    TEST_ASSERT(std::abs(norm.length() - 1.0f) < 0.001f, "Normalized vector should have length 1");

    Vec2 zero(0.0f, 0.0f);
    Vec2 zeroNorm = zero.normalized();
    TEST_ASSERT(zeroNorm.x == 0.0f && zeroNorm.y == 0.0f, "Zero vector normalized should be zero");

    testsPassed++;
}

void test_directions() {
    TEST_ASSERT(oppositeDirection(Direction::Up) == Direction::Down, "Opposite of Up is Down");
    TEST_ASSERT(oppositeDirection(Direction::Down) == Direction::Up, "Opposite of Down is Up");
    TEST_ASSERT(oppositeDirection(Direction::Left) == Direction::Right, "Opposite of Left is Right");
    TEST_ASSERT(oppositeDirection(Direction::Right) == Direction::Left, "Opposite of Right is Left");

    Vec2 upVec = Vec2::fromDirection(Direction::Up);
    TEST_ASSERT(upVec.x == 0.0f && upVec.y == -1.0f, "Up vector should be (0, -1)");

    Vec2 downVec = Vec2::fromDirection(Direction::Down);
    TEST_ASSERT(downVec.x == 0.0f && downVec.y == 1.0f, "Down vector should be (0, 1)");

    testsPassed++;
}

void test_rect_overlap() {
    Rect r1(0.0f, 0.0f, 16.0f, 16.0f);
    Rect r2(10.0f, 10.0f, 16.0f, 16.0f);
    Rect r3(20.0f, 20.0f, 16.0f, 16.0f);

    TEST_ASSERT(r1.overlaps(r2), "r1 and r2 should overlap");
    TEST_ASSERT(r2.overlaps(r1), "Overlap should be commutative");
    TEST_ASSERT(!r1.overlaps(r3), "r1 and r3 should not overlap");

    TEST_ASSERT(r1.contains(Vec2(8.0f, 8.0f)), "r1 should contain (8, 8)");
    TEST_ASSERT(!r1.contains(Vec2(18.0f, 8.0f)), "r1 should not contain (18, 8)");

    testsPassed++;
}

void test_game_config() {
    TEST_ASSERT(GameConfig::VIRTUAL_WIDTH == 320, "Virtual width must be 320");
    TEST_ASSERT(GameConfig::VIRTUAL_HEIGHT == 240, "Virtual height must be 240");
    TEST_ASSERT(GameConfig::WORLD_WIDTH == 640, "World width must be 640");
    TEST_ASSERT(GameConfig::WORLD_HEIGHT == 480, "World height must be 480");
    TEST_ASSERT(GameConfig::INITIAL_HEALTH == 6, "Initial health must be 6 (3 full hearts)");
    TEST_ASSERT(GameConfig::HURT_INVULNERABILITY_SECS == 1.0f, "Hurt invulnerability must be 1.0s");

    testsPassed++;
}

void test_sound_generator() {
    const Core::SoundPreset presets[] = {
        Core::SoundPreset::GameStart,
        Core::SoundPreset::SwordSlash,
        Core::SoundPreset::ArrowShoot,
        Core::SoundPreset::Hit,
        Core::SoundPreset::EnemyDefeat,
        Core::SoundPreset::SecretChime,
        Core::SoundPreset::ChestFanfare,
        Core::SoundPreset::PlayerHurt,
        Core::SoundPreset::BossRoar,
    };

    for (auto preset : presets) {
        Core::SoundParams params = Core::getPresetParams(preset);
        auto samples = Core::generateAudioSamples(params, 44100);
        TEST_ASSERT(!samples.empty(), "Generated samples should not be empty");

        bool hasSignal = false;
        for (float s : samples) {
            TEST_ASSERT(s >= -1.0f && s <= 1.0f, "Samples must be clipped within [-1.0, 1.0]");
            if (std::abs(s) > 0.01f) {
                hasSignal = true;
            }
        }
        TEST_ASSERT(hasSignal, "Audio waveform must contain audible signal");
    }

    testsPassed++;
}

void test_generated_assets() {
    const auto& assets = Render::getAllGeneratedAssets();
    TEST_ASSERT(assets.size() == 22, "Must contain exactly 22 procedural pixel art assets");

    for (const auto& asset : assets) {
        TEST_ASSERT(asset.width == 16, "Asset width must be 16");
        TEST_ASSERT(asset.height == 16, "Asset height must be 16");
    }

    TEST_ASSERT(Render::findGeneratedAsset("hero_down_0") != nullptr, "hero_down_0 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("grass") != nullptr, "grass must exist");
    TEST_ASSERT(Render::findGeneratedAsset("heart_full") != nullptr, "heart_full must exist");
    TEST_ASSERT(Render::findGeneratedAsset("shrine_pedestal_sword") != nullptr, "shrine_pedestal_sword must exist");
    TEST_ASSERT(Render::findGeneratedAsset("slash_down") != nullptr, "slash_down must exist");
    TEST_ASSERT(Render::findGeneratedAsset("non_existent_sprite") == nullptr, "non_existent_sprite must return nullptr");

    testsPassed++;
}

void test_movement_math() {
    // Cardinal moving down
    MovementInput inDown{false, true, false, false};
    auto resDown = computeMovementVelocity(inDown, Direction::Up, 100.0f);
    TEST_ASSERT(resDown.isMoving, "Should be moving down");
    TEST_ASSERT(resDown.facing == Direction::Down, "Facing should be Down");
    TEST_ASSERT(resDown.velocity.x == 0.0f && resDown.velocity.y == 100.0f, "Velocity should be (0, 100)");

    // Diagonal moving up-right
    MovementInput inDiag{true, false, false, true};
    auto resDiag = computeMovementVelocity(inDiag, Direction::Down, 100.0f);
    TEST_ASSERT(resDiag.isMoving, "Should be moving diagonally");
    TEST_ASSERT(resDiag.facing == Direction::Up, "Vertical direction takes precedence or valid facing");
    float expectedDiag = 100.0f * 0.70710678f;
    TEST_ASSERT(std::abs(resDiag.velocity.x - expectedDiag) < 0.01f, "Diagonal X should be normalized");
    TEST_ASSERT(std::abs(resDiag.velocity.y - (-expectedDiag)) < 0.01f, "Diagonal Y should be normalized");

    // Standing still retains current facing
    MovementInput inNone{false, false, false, false};
    auto resNone = computeMovementVelocity(inNone, Direction::Left, 100.0f);
    TEST_ASSERT(!resNone.isMoving, "Should not be moving");
    TEST_ASSERT(resNone.facing == Direction::Left, "Facing should be retained when stopped");
    TEST_ASSERT(resNone.velocity.x == 0.0f && resNone.velocity.y == 0.0f, "Velocity should be zero");

    // Opposing inputs cancel
    MovementInput inCancel{true, true, true, true};
    auto resCancel = computeMovementVelocity(inCancel, Direction::Right, 100.0f);
    TEST_ASSERT(!resCancel.isMoving, "Opposing inputs should cancel out");

    testsPassed++;
}

void test_map_data() {
    MapData map;
    TEST_ASSERT(map.isSolid(0, 0), "Perimeter (0, 0) must be solid");
    TEST_ASSERT(map.isSolid(1, 1), "Perimeter (1, 1) must be solid");
    TEST_ASSERT(map.isSolid(38, 28), "Perimeter (38, 28) must be solid");
    TEST_ASSERT(map.getObstacle(0, 0) == ObstacleTile::Tree, "Perimeter should be Tree");

    // Pond
    TEST_ASSERT(map.isSolid(26, 4), "Pond (26, 4) must be solid water");
    TEST_ASSERT(map.getObstacle(26, 4) == ObstacleTile::Water, "(26, 4) should be Water");

    // Clear path
    TEST_ASSERT(!map.isSolid(12, 17), "Path (12, 17) should not be solid");
    TEST_ASSERT(map.getGround(12, 17) == GroundTile::Path, "(12, 17) should be Path");

    // Out of bounds is solid
    TEST_ASSERT(map.isSolid(-1, 0), "Negative coords must be solid");
    TEST_ASSERT(map.isSolid(100, 100), "Coords > bounds must be solid");

    // Rect collision
    Rect clearBox(12.0f * 16.0f, 17.0f * 16.0f, 16.0f, 16.0f);
    TEST_ASSERT(!map.checkRectCollision(clearBox), "Open path tile must not trigger collision");

    Rect solidBox(0.0f, 0.0f, 16.0f, 16.0f);
    TEST_ASSERT(map.checkRectCollision(solidBox), "Solid perimeter tile must trigger collision");

    testsPassed++;
}

void test_player_simulation() {
    MapData map;
    Player player(200.0f, 280.0f);

    TEST_ASSERT(player.getPosition() == Vec2(200.0f, 280.0f), "Initial position must be (200, 280)");
    TEST_ASSERT(player.getFacing() == Direction::Down, "Initial facing must be Down");
    TEST_ASSERT(player.getTextureName() == "hero_down_0", "Initial texture must be hero_down_0");

    // Move player right on open path for 0.1s
    MovementInput inRight{false, false, false, true};
    player.update(0.1f, inRight, map);
    TEST_ASSERT(player.getPosition().x > 200.0f, "Player should have moved right");
    TEST_ASSERT(player.getFacing() == Direction::Right, "Facing should be Right");

    // Step animation toggles after FRAME_DURATION (0.15s)
    player.update(0.1f, inRight, map);
    TEST_ASSERT(player.getTextureName() == "hero_right_1", "Texture should advance to frame 1 while moving");

    // Stop moving: resets to frame 0
    MovementInput inNone{false, false, false, false};
    player.update(0.01f, inNone, map);
    TEST_ASSERT(!player.isMoving(), "Player should be idle");
    TEST_ASSERT(player.getTextureName() == "hero_right_0", "Texture should reset to frame 0 when idle");

    // Collision stop: place player near left tree border (x = 32)
    player.setPosition(Vec2(33.0f, 200.0f));
    MovementInput inLeft{false, false, true, false};
    // Move left towards solid tree border
    for (int i = 0; i < 20; ++i) {
        player.update(0.05f, inLeft, map);
    }
    // Player hitbox width=12, x=2, solid trees at x < 32 => player cannot penetrate tree
    TEST_ASSERT(player.getWorldHitbox().left() >= 32.0f, "Player must not penetrate solid trees");

    testsPassed++;
}

int main() {
    std::cout << "=== Running Headless Core Tests ===" << std::endl;
    test_vector_math();
    test_directions();
    test_rect_overlap();
    test_game_config();
    test_sound_generator();
    test_generated_assets();
    test_movement_math();
    test_map_data();
    test_player_simulation();

    std::cout << "Tests Passed: " << testsPassed << " | Tests Failed: " << testsFailed << std::endl;
    return testsFailed == 0 ? 0 : 1;
}
