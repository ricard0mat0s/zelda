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
#include "../src/core/Health.hpp"
#include "../src/core/Inventory.hpp"
#include "../src/core/Typewriter.hpp"
#include "../src/core/Combat.hpp"
#include "../src/core/Shield.hpp"
#include "../src/core/Slime.hpp"
#include "../src/core/ItemDrop.hpp"
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
    TEST_ASSERT(assets.size() == 24, "Must contain exactly 24 procedural pixel art assets");

    for (const auto& asset : assets) {
        TEST_ASSERT(asset.width == 16, "Asset width must be 16");
        TEST_ASSERT(asset.height == 16, "Asset height must be 16");
    }

    TEST_ASSERT(Render::findGeneratedAsset("hero_down_0") != nullptr, "hero_down_0 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("grass") != nullptr, "grass must exist");
    TEST_ASSERT(Render::findGeneratedAsset("heart_full") != nullptr, "heart_full must exist");
    TEST_ASSERT(Render::findGeneratedAsset("shrine_pedestal_sword") != nullptr, "shrine_pedestal_sword must exist");
    TEST_ASSERT(Render::findGeneratedAsset("slash_down") != nullptr, "slash_down must exist");
    TEST_ASSERT(Render::findGeneratedAsset("slime_0") != nullptr, "slime_0 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("slime_1") != nullptr, "slime_1 must exist");
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

void test_health_system() {
    Health health(3);
    TEST_ASSERT(health.getMaxHearts() == 3, "Max hearts should be 3");
    TEST_ASSERT(health.getMaxHP() == 6, "Max HP should be 6");
    TEST_ASSERT(health.getCurrentHP() == 6, "Current HP should be 6");
    TEST_ASSERT(!health.isDead(), "Health should not be dead");
    TEST_ASSERT(!health.isInvulnerable(), "Health should not be invulnerable initially");

    auto states = health.getHeartStates();
    TEST_ASSERT(states.size() == 3, "Should have 3 heart states");
    TEST_ASSERT(states[0] == HeartState::Full && states[1] == HeartState::Full && states[2] == HeartState::Full, "All 3 hearts should be Full");

    // Take 1 HP damage (half heart)
    auto res1 = health.takeDamage(1);
    TEST_ASSERT(res1.damaged, "Should have taken damage");
    TEST_ASSERT(res1.remainingHP == 5, "Remaining HP should be 5");
    TEST_ASSERT(health.isInvulnerable(), "Should be invulnerable after damage");

    states = health.getHeartStates();
    TEST_ASSERT(states[0] == HeartState::Full && states[1] == HeartState::Full && states[2] == HeartState::Half, "Heart states should be Full, Full, Half");

    // Taking damage while invulnerable is blocked
    auto resBlocked = health.takeDamage(2);
    TEST_ASSERT(!resBlocked.damaged, "Damage must be ignored while invulnerable");
    TEST_ASSERT(health.getCurrentHP() == 5, "HP must remain 5");

    // Advance time past 1.0s invulnerability
    health.update(1.1f);
    TEST_ASSERT(!health.isInvulnerable(), "Invulnerability should expire after 1.1s");

    // Take 4 HP damage
    health.takeDamage(4);
    TEST_ASSERT(health.getCurrentHP() == 1, "Remaining HP should be 1");
    states = health.getHeartStates();
    TEST_ASSERT(states[0] == HeartState::Half && states[1] == HeartState::Empty && states[2] == HeartState::Empty, "Heart states should be Half, Empty, Empty");

    health.update(1.1f);

    // Heal 3 HP
    health.heal(3);
    TEST_ASSERT(health.getCurrentHP() == 4, "HP should be 4 after healing 3");
    states = health.getHeartStates();
    TEST_ASSERT(states[0] == HeartState::Full && states[1] == HeartState::Full && states[2] == HeartState::Empty, "Heart states should be Full, Full, Empty");

    // Add heart container
    health.addHeartContainer();
    TEST_ASSERT(health.getMaxHearts() == 4, "Max hearts should now be 4");
    TEST_ASSERT(health.getCurrentHP() == 8, "Adding heart container should restore full 8 HP");
    states = health.getHeartStates();
    TEST_ASSERT(states.size() == 4, "Should have 4 heart containers");
    TEST_ASSERT(states[3] == HeartState::Full, "Fourth heart should be Full");

    health.update(1.1f);

    // Fatal damage
    auto resFatal = health.takeDamage(10);
    TEST_ASSERT(resFatal.damaged && resFatal.isDead, "Fatal damage should kill");
    TEST_ASSERT(health.isDead(), "Should be dead");
    TEST_ASSERT(health.getCurrentHP() == 0, "HP should be 0");

    // Cannot heal while dead
    health.heal(10);
    TEST_ASSERT(health.getCurrentHP() == 0 && health.isDead(), "Cannot heal while dead");

    testsPassed++;
}

void test_player_damage_and_flicker() {
    Player player(200.0f, 280.0f);
    TEST_ASSERT(player.getHealth().getCurrentHP() == 6, "Player starts with 6 HP");

    // Take damage
    auto res = player.takeDamage(2);
    TEST_ASSERT(res.damaged, "Player should take damage");
    TEST_ASSERT(player.getHealth().getCurrentHP() == 4, "Player HP should be 4");
    TEST_ASSERT(player.getHealth().isInvulnerable(), "Player should be invulnerable");

    // Flicker test
    bool initialFlicker = player.getHealth().isFlickerVisible();
    player.getHealth().update(0.08f);
    bool nextFlicker = player.getHealth().isFlickerVisible();
    TEST_ASSERT(initialFlicker != nextFlicker, "Flicker visibility should alternate across 0.08s steps");

    // Expire invulnerability
    player.getHealth().update(1.0f);
    TEST_ASSERT(player.getHealth().isFlickerVisible(), "Flicker should always be visible when not invulnerable");

    // Fatal damage to player
    player.takeDamage(10);
    TEST_ASSERT(player.getHealth().isDead(), "Player should be dead");

    MapData map;
    MovementInput inRight{false, false, false, true};
    player.update(0.1f, inRight, map);
    TEST_ASSERT(player.getVelocity().x == 0.0f && !player.isMoving(), "Dead player must not move");

    testsPassed++;
}

void test_inventory() {
    Inventory inv;
    TEST_ASSERT(!inv.hasSword, "Should not start with sword");
    TEST_ASSERT(!inv.hasBow, "Should not start with bow");
    TEST_ASSERT(inv.keys == 0, "Should start with 0 keys");

    inv.obtainSword();
    TEST_ASSERT(inv.hasSword, "Should now have sword");

    inv.addKeys(2);
    TEST_ASSERT(inv.keys == 2, "Should have 2 keys");

    TEST_ASSERT(inv.useKey(), "First key use should succeed");
    TEST_ASSERT(inv.keys == 1, "Keys should be 1");
    TEST_ASSERT(inv.useKey(), "Second key use should succeed");
    TEST_ASSERT(inv.keys == 0, "Keys should be 0");
    TEST_ASSERT(!inv.useKey(), "Key use with 0 keys should fail");

    testsPassed++;
}

void test_typewriter() {
    Typewriter tw("Hello", 0.02f);
    TEST_ASSERT(tw.getDisplayedText().empty(), "Initially no characters revealed");
    TEST_ASSERT(!tw.isComplete(), "Should not be complete initially");

    // Advance 0.04s -> 2 chars ("He")
    tw.update(0.045f);
    TEST_ASSERT(tw.getDisplayedText() == "He", "Should reveal 2 characters 'He'");

    // Skip to end
    tw.skip();
    TEST_ASSERT(tw.isComplete(), "Should be complete after skip");
    TEST_ASSERT(tw.getDisplayedText() == "Hello", "Should reveal entire string on skip");

    testsPassed++;
}

void test_combat_and_sword_hitboxes() {
    Vec2 playerPos(100.0f, 100.0f);

    Rect hitDown = getSwordHitbox(playerPos, Direction::Down);
    TEST_ASSERT(hitDown.x == 100.0f && hitDown.y == 114.0f, "Sword Down hitbox must be in front of player");
    TEST_ASSERT(hitDown.width == 16.0f && hitDown.height == 14.0f, "Sword Down hitbox dimensions");

    Rect hitUp = getSwordHitbox(playerPos, Direction::Up);
    TEST_ASSERT(hitUp.x == 100.0f && hitUp.y == 86.0f, "Sword Up hitbox must be above player");

    Rect hitLeft = getSwordHitbox(playerPos, Direction::Left);
    TEST_ASSERT(hitLeft.x == 86.0f && hitLeft.y == 100.0f, "Sword Left hitbox must be left of player");

    Rect hitRight = getSwordHitbox(playerPos, Direction::Right);
    TEST_ASSERT(hitRight.x == 116.0f && hitRight.y == 100.0f, "Sword Right hitbox must be right of player");

    // Knockback
    Vec2 attacker(100.0f, 100.0f);
    Vec2 targetDown(100.0f, 120.0f);
    Vec2 kbDown = calculateKnockback(attacker, targetDown, 140.0f);
    TEST_ASSERT(kbDown.x == 0.0f && kbDown.y == 140.0f, "Knockback should be directly downward");

    Vec2 targetRight(130.0f, 100.0f);
    Vec2 kbRight = calculateKnockback(attacker, targetRight, 140.0f);
    TEST_ASSERT(kbRight.x == 140.0f && kbRight.y == 0.0f, "Knockback should be directly rightward");

    testsPassed++;
}

void test_player_sword_attack() {
    Player player(200.0f, 280.0f);
    MapData map;

    // Cannot attack without sword
    TEST_ASSERT(!player.getInventory().hasSword, "Player starts without sword");
    TEST_ASSERT(!player.startAttack(), "Player cannot attack without sword");
    TEST_ASSERT(!player.isAttacking(), "Player should not be attacking");

    // Obtain sword and attack
    player.getInventory().obtainSword();
    TEST_ASSERT(player.startAttack(), "Player can attack with sword");
    TEST_ASSERT(player.isAttacking(), "Player is now attacking");

    // While attacking, player cannot move
    MovementInput inRight{false, false, false, true};
    player.update(0.1f, inRight, map);
    TEST_ASSERT(player.getPosition().x == 200.0f, "Player position must stay locked during swing");
    TEST_ASSERT(player.getCurrentSwordHitbox().width > 0.0f, "Active sword hitbox should be present");

    // After attack duration ends, player can move again
    player.update(0.2f, inRight, map);
    TEST_ASSERT(!player.isAttacking(), "Attack state should clear after timer expires");

    // Now movement works again
    player.update(0.1f, inRight, map);
    TEST_ASSERT(player.getPosition().x > 200.0f, "Player should move after attack ends");

    testsPassed++;
}

void test_shield_defense() {
    Vec2 playerPos(100.0f, 100.0f);

    // Facing Down: threats from below are blocked, threats from behind or side are not
    TEST_ASSERT(isAttackBlockedByShield(playerPos, Direction::Down, Vec2(100.0f, 120.0f)), "Head-on threat from below must be blocked");
    TEST_ASSERT(isAttackBlockedByShield(playerPos, Direction::Down, Vec2(110.0f, 120.0f)), "Slightly diagonal threat from below must be blocked");
    TEST_ASSERT(!isAttackBlockedByShield(playerPos, Direction::Down, Vec2(100.0f, 80.0f)), "Threat from behind (above) must NOT be blocked");
    TEST_ASSERT(!isAttackBlockedByShield(playerPos, Direction::Down, Vec2(80.0f, 100.0f)), "Threat from left side must NOT be blocked");

    // Facing Up: threats from above are blocked
    TEST_ASSERT(isAttackBlockedByShield(playerPos, Direction::Up, Vec2(100.0f, 80.0f)), "Threat from above must be blocked");
    TEST_ASSERT(!isAttackBlockedByShield(playerPos, Direction::Up, Vec2(100.0f, 120.0f)), "Threat from below must NOT be blocked");

    // Facing Left: threats from left are blocked
    TEST_ASSERT(isAttackBlockedByShield(playerPos, Direction::Left, Vec2(80.0f, 100.0f)), "Threat from left must be blocked");

    // Facing Right: threats from right are blocked
    TEST_ASSERT(isAttackBlockedByShield(playerPos, Direction::Right, Vec2(120.0f, 100.0f)), "Threat from right must be blocked");

    testsPassed++;
}

void test_slime_ai_and_combat() {
    Slime slime(150.0f, 150.0f);
    TEST_ASSERT(slime.getHP() == 2, "Slime starts with 2 HP");
    TEST_ASSERT(!slime.isDead(), "Slime starts alive");
    TEST_ASSERT(!slime.isHurt(), "Slime starts not hurt");

    // Take 1 damage with knockback
    Vec2 kb(0.0f, 100.0f);
    bool tookDmg = slime.takeDamage(1, kb);
    TEST_ASSERT(tookDmg, "First hit must inflict damage");
    TEST_ASSERT(slime.getHP() == 1, "Slime HP must be 1 after hit");
    TEST_ASSERT(slime.isHurt(), "Slime must be in hurt state");
    TEST_ASSERT(!slime.isDead(), "Slime should still be alive");

    // Immunity during hurt window
    bool tookDmgBlocked = slime.takeDamage(1, kb);
    TEST_ASSERT(!tookDmgBlocked, "Damage during hurt window must be blocked");

    // Advance past hurt timer
    MapData map;
    slime.update(0.3f, Vec2(200.0f, 200.0f), map);
    TEST_ASSERT(!slime.isHurt(), "Hurt state must clear after timer");

    // Fatal blow
    bool fatal = slime.takeDamage(1, kb);
    TEST_ASSERT(fatal, "Second hit must inflict damage");
    TEST_ASSERT(slime.isDead(), "Slime must be dead at 0 HP");

    // Dead slime cannot take damage
    TEST_ASSERT(!slime.takeDamage(1, kb), "Dead slime cannot take damage");

    testsPassed++;
}

void test_item_drops() {
    ItemDrop drop;
    drop.pos = Vec2(100.0f, 100.0f);
    TEST_ASSERT(drop.active, "Item drop starts active");
    TEST_ASSERT(!drop.isBlinking(), "Should not blink initially");

    // Advance to blink phase (< 3s remaining)
    drop.update(10.0f);
    TEST_ASSERT(drop.active, "Item should still be active");
    TEST_ASSERT(drop.lifetime == 2.0f, "Lifetime should be 2.0s");

    // Advance past lifetime -> deactivates
    drop.update(2.5f);
    TEST_ASSERT(!drop.active, "Item should be inactive after lifetime expires");

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
    test_health_system();
    test_player_damage_and_flicker();
    test_inventory();
    test_typewriter();
    test_combat_and_sword_hitboxes();
    test_player_sword_attack();
    test_shield_defense();
    test_slime_ai_and_combat();
    test_item_drops();

    std::cout << "Tests Passed: " << testsPassed << " | Tests Failed: " << testsFailed << std::endl;
    return testsFailed == 0 ? 0 : 1;
}
