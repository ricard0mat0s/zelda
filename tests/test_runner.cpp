#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../src/core/types.hpp"
#include "../src/core/GameConfig.hpp"

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

int main() {
    std::cout << "=== Running Headless Core Tests ===" << std::endl;
    test_vector_math();
    test_directions();
    test_rect_overlap();
    test_game_config();

    std::cout << "Tests Passed: " << testsPassed << " | Tests Failed: " << testsFailed << std::endl;
    return testsFailed == 0 ? 0 : 1;
}
