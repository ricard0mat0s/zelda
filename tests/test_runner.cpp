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
#include "../src/core/Dungeon.hpp"
#include "../src/core/Projectile.hpp"
#include "../src/core/DungeonEnemies.hpp"
#include "../src/core/BossGolem.hpp"
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
    TEST_ASSERT(assets.size() == 52, "Must contain exactly 52 procedural pixel art assets");

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
    TEST_ASSERT(Render::findGeneratedAsset("dungeon_floor") != nullptr, "dungeon_floor must exist");
    TEST_ASSERT(Render::findGeneratedAsset("dungeon_wall") != nullptr, "dungeon_wall must exist");
    TEST_ASSERT(Render::findGeneratedAsset("dungeon_door_closed") != nullptr, "dungeon_door_closed must exist");
    TEST_ASSERT(Render::findGeneratedAsset("dungeon_door_open") != nullptr, "dungeon_door_open must exist");
    TEST_ASSERT(Render::findGeneratedAsset("pressure_plate_up") != nullptr, "pressure_plate_up must exist");
    TEST_ASSERT(Render::findGeneratedAsset("pressure_plate_down") != nullptr, "pressure_plate_down must exist");
    TEST_ASSERT(Render::findGeneratedAsset("dungeon_stairs") != nullptr, "dungeon_stairs must exist");
    TEST_ASSERT(Render::findGeneratedAsset("chest_closed") != nullptr, "chest_closed must exist");
    TEST_ASSERT(Render::findGeneratedAsset("chest_open") != nullptr, "chest_open must exist");
    TEST_ASSERT(Render::findGeneratedAsset("bow_icon") != nullptr, "bow_icon must exist");
    TEST_ASSERT(Render::findGeneratedAsset("arrow_up") != nullptr, "arrow_up must exist");
    TEST_ASSERT(Render::findGeneratedAsset("arrow_down") != nullptr, "arrow_down must exist");
    TEST_ASSERT(Render::findGeneratedAsset("arrow_left") != nullptr, "arrow_left must exist");
    TEST_ASSERT(Render::findGeneratedAsset("arrow_right") != nullptr, "arrow_right must exist");
    TEST_ASSERT(Render::findGeneratedAsset("crystal_switch_blue") != nullptr, "crystal_switch_blue must exist");
    TEST_ASSERT(Render::findGeneratedAsset("crystal_switch_active") != nullptr, "crystal_switch_active must exist");
    TEST_ASSERT(Render::findGeneratedAsset("chasm") != nullptr, "chasm must exist");
    TEST_ASSERT(Render::findGeneratedAsset("bat_0") != nullptr, "bat_0 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("bat_1") != nullptr, "bat_1 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("skeleton_0") != nullptr, "skeleton_0 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("skeleton_1") != nullptr, "skeleton_1 must exist");
    TEST_ASSERT(Render::findGeneratedAsset("small_key") != nullptr, "small_key must exist");
    TEST_ASSERT(Render::findGeneratedAsset("dungeon_door_locked") != nullptr, "dungeon_door_locked must exist");
    TEST_ASSERT(Render::findGeneratedAsset("boss_golem_armored") != nullptr, "boss_golem_armored must exist");
    TEST_ASSERT(Render::findGeneratedAsset("boss_golem_charging") != nullptr, "boss_golem_charging must exist");
    TEST_ASSERT(Render::findGeneratedAsset("boss_golem_stunned") != nullptr, "boss_golem_stunned must exist");
    TEST_ASSERT(Render::findGeneratedAsset("rock_projectile") != nullptr, "rock_projectile must exist");
    TEST_ASSERT(Render::findGeneratedAsset("heart_container") != nullptr, "heart_container must exist");
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

void test_dungeon_room_layout_and_plate() {
    Dungeon dungeon;
    dungeon.init();

    TEST_ASSERT(dungeon.getCurrentRoomId() == 1, "Starts in Room 1 (Entrance Foyer)");
    const auto& r1 = dungeon.getCurrentRoom();
    TEST_ASSERT(r1.gridX == 1 && r1.gridY == 2, "Room 1 grid should be (1, 2)");
    TEST_ASSERT(r1.hasPlate, "Room 1 must have a pressure plate");
    TEST_ASSERT(r1.hasStairs, "Room 1 must have stairs to overworld");
    TEST_ASSERT(r1.getDoor(Direction::Up) == DoorState::Closed, "North door starts closed");

    // Check closed North door blocks movement
    Rect doorwayBox{150.0f, 10.0f, 12.0f, 10.0f};
    TEST_ASSERT(dungeon.checkCollision(doorwayBox), "Closed north door must collide / block player");

    // Stepping on pressure plate at (160, 112)
    Rect plateStepBox{158.0f, 110.0f, 10.0f, 10.0f};
    bool triggered = dungeon.checkPlateTrigger(plateStepBox);
    TEST_ASSERT(triggered, "Pressure plate must trigger when stepped on");
    TEST_ASSERT(r1.plate.pressed, "Pressure plate must be marked pressed");
    TEST_ASSERT(r1.getDoor(Direction::Up) == DoorState::Open, "North door must unlock to Open");

    // Step again should not trigger again
    TEST_ASSERT(!dungeon.checkPlateTrigger(plateStepBox), "Second step on plate does not re-trigger");

    // Now North door is open, so doorway allows passage
    TEST_ASSERT(!dungeon.checkCollision(doorwayBox), "Open north door allows passage");

    // Stairs exit check
    Rect stairsStepBox{158.0f, 206.0f, 10.0f, 10.0f};
    TEST_ASSERT(dungeon.checkStairsExit(stairsStepBox), "Stepping on stairs triggers exit");

    testsPassed++;
}

void test_dungeon_room_transitions() {
    Dungeon dungeon;
    dungeon.init();

    // Unlock North door
    Rect plateStepBox{158.0f, 110.0f, 10.0f, 10.0f};
    dungeon.checkPlateTrigger(plateStepBox);

    // Link approaches North doorway
    Vec2 playerPos{160.0f, 16.0f};
    Rect playerHitbox{154.0f, 12.0f, 12.0f, 10.0f};
    bool transitionStarted = dungeon.checkDoorwayTransition(playerHitbox, Direction::Up, playerPos);
    TEST_ASSERT(transitionStarted, "Walking into open north doorway initiates room transition");
    TEST_ASSERT(dungeon.isTransitioning(), "Dungeon must be in transitioning state");

    const auto& trans = dungeon.getTransition();
    TEST_ASSERT(trans.fromRoomId == 1, "Transition from room 1");
    TEST_ASSERT(trans.toRoomId == 2, "Transition to room 2");
    TEST_ASSERT(trans.direction == Direction::Up, "Transition direction is Up");

    // Test camera interpolation during transition
    dungeon.update(0.25f);
    TEST_ASSERT(dungeon.isTransitioning(), "Still transitioning at halfway mark");
    Vec2 midCamera = dungeon.getCameraOffset();
    TEST_ASSERT(midCamera.y < trans.startCameraPos.y, "Camera should interpolate upwards");

    // Complete transition (0.5s duration)
    dungeon.update(0.3f);
    TEST_ASSERT(!dungeon.isTransitioning(), "Transition finished after duration");
    TEST_ASSERT(dungeon.getCurrentRoomId() == 2, "Current room is now Room 2 (Crystal Chasm)");

    // Test transition from Room 2 to Room 3 (West)
    Vec2 westPlayerPos{16.0f, 120.0f};
    Rect westPlayerHitbox{10.0f, 115.0f, 12.0f, 10.0f};
    bool westTransition = dungeon.checkDoorwayTransition(westPlayerHitbox, Direction::Left, westPlayerPos);
    TEST_ASSERT(westTransition, "Walking into west doorway from Room 2 initiates transition to Room 3");
    dungeon.update(0.6f);
    TEST_ASSERT(dungeon.getCurrentRoomId() == 3, "Current room is now Room 3 (Treasure Chamber)");

    testsPassed++;
}

void test_arrow_projectile() {
    // 1. Arrow firing in 4 directions
    Arrow arrowUp(Vec2(100.0f, 100.0f), Direction::Up, 200.0f);
    TEST_ASSERT(arrowUp.isActive(), "Arrow starts active");
    TEST_ASSERT(arrowUp.getTextureName() == "arrow_up", "Up arrow texture name");

    arrowUp.update(0.1f);
    TEST_ASSERT(arrowUp.getPosition().y == 80.0f, "Arrow moved up by 20px in 0.1s");

    Arrow arrowRight(Vec2(50.0f, 50.0f), Direction::Right, 100.0f);
    TEST_ASSERT(arrowRight.getTextureName() == "arrow_right", "Right arrow texture name");
    arrowRight.update(0.5f);
    TEST_ASSERT(arrowRight.getPosition().x == 100.0f, "Arrow moved right by 50px");

    // 2. Lifetime expiration
    arrowRight.update(2.0f);
    TEST_ASSERT(!arrowRight.isActive(), "Arrow deactivates after lifetime expires");

    // 3. Deactivate manually on hit
    arrowUp.deactivate();
    TEST_ASSERT(!arrowUp.isActive(), "Arrow deactivates manually");

    testsPassed++;
}

void test_treasure_chest_in_room_3() {
    Dungeon dungeon;
    dungeon.init();
    dungeon.setCurrentRoomId(3); // Treasure Chamber

    const auto& room3 = dungeon.getCurrentRoom();
    TEST_ASSERT(room3.hasChest, "Room 3 must contain a treasure chest");
    TEST_ASSERT(!room3.chest.opened, "Chest starts closed");

    // Check chest blocks player movement when closed
    Rect chestBox = room3.chest.getHitbox();
    TEST_ASSERT(dungeon.checkCollision(chestBox), "Chest collision must block player");

    // Player far away cannot open chest
    Vec2 farPlayerPos(50.0f, 50.0f);
    TEST_ASSERT(!dungeon.checkChestInteract(farPlayerPos, Direction::Up), "Cannot open chest from afar");

    // Player standing directly below chest facing Up opens chest
    Vec2 nearPlayerPos(160.0f, 114.0f);
    bool opened = dungeon.checkChestInteract(nearPlayerPos, Direction::Up);
    TEST_ASSERT(opened, "Interacting with chest opens it");
    TEST_ASSERT(room3.chest.opened, "Chest is now marked opened");

    // Re-interacting cannot open again
    TEST_ASSERT(!dungeon.checkChestInteract(nearPlayerPos, Direction::Up), "Cannot open already opened chest");

    testsPassed++;
}

void test_crystal_switch_arrow_trigger() {
    Dungeon dungeon;
    dungeon.init();
    dungeon.setCurrentRoomId(2); // Crystal Chasm

    const auto& room2 = dungeon.getCurrentRoom();
    TEST_ASSERT(room2.hasCrystalSwitch, "Room 2 must contain a crystal switch");
    TEST_ASSERT(!room2.crystalSwitch.active, "Crystal switch starts inactive");
    TEST_ASSERT(room2.getDoor(Direction::Up) == DoorState::Closed, "North door starts closed");

    // Verify chasm pit blocks walking
    Rect pitBox{50.0f, 70.0f, 12.0f, 12.0f};
    TEST_ASSERT(dungeon.checkCollision(pitBox), "Chasm pit must block player walking");

    // But projectiles can fly over chasm pits!
    TEST_ASSERT(!dungeon.checkProjectileCollision(pitBox), "Arrows can fly over chasm pits");

    // Fire arrow into crystal switch
    Arrow arrow(Vec2(160.0f, 120.0f), Direction::Up, 240.0f);
    // Switch is at (160, 48). When arrow reaches (160, 48):
    Arrow hittingArrow(Vec2(156.0f, 46.0f), Direction::Up);
    bool hit = dungeon.checkCrystalSwitchHit(hittingArrow.getHitbox());
    TEST_ASSERT(hit, "Arrow striking crystal switch triggers activation");
    TEST_ASSERT(room2.crystalSwitch.active, "Crystal switch is now active");
    TEST_ASSERT(room2.getDoor(Direction::Up) == DoorState::Open, "North door unlocks to Open when switch is struck");

    // Second hit does not re-trigger
    TEST_ASSERT(!dungeon.checkCrystalSwitchHit(hittingArrow.getHitbox()), "Second hit does not re-trigger");

    testsPassed++;
}

void test_bat_movement_and_damage() {
    Bat bat(100.0f, 100.0f);
    TEST_ASSERT(bat.getHP() == 1, "Bat initial HP must be 1");
    TEST_ASSERT(!bat.isDead(), "Bat must start alive");
    TEST_ASSERT(!bat.isHurt(), "Bat must not start hurt");
    TEST_ASSERT(bat.getTextureName() == "bat_0", "Initial texture is bat_0");

    // Animation frame changes after 0.12s
    bat.update(0.15f, Vec2(250.0f, 250.0f));
    TEST_ASSERT(bat.getTextureName() == "bat_1", "Bat texture alternates on flapping");

    // Close player triggers swoop
    Vec2 closePlayer(110.0f, 100.0f);
    bat.update(0.1f, closePlayer);
    TEST_ASSERT(bat.getHitbox().width == 12.0f && bat.getHitbox().height == 10.0f, "Bat hitbox valid");

    // Take damage
    bool hit = bat.takeDamage(1);
    TEST_ASSERT(hit, "Bat takes damage");
    TEST_ASSERT(bat.isDead(), "Bat dies with 1 damage");
    TEST_ASSERT(bat.getHP() <= 0, "Bat HP is 0 or less");
    TEST_ASSERT(!bat.takeDamage(1), "Dead bat cannot take further damage");

    testsPassed++;
}

void test_skeleton_patrol_chase_and_knockback() {
    Dungeon dungeon;
    dungeon.init();

    Skeleton skel(160.0f, 120.0f);
    TEST_ASSERT(skel.getHP() == 3, "Skeleton initial HP must be 3");
    TEST_ASSERT(!skel.isDead(), "Skeleton must start alive");
    TEST_ASSERT(!skel.isHurt(), "Skeleton must not start hurt");
    TEST_ASSERT(skel.getFacing() == Direction::Down, "Initial facing is Down");

    // Player far away -> idle
    skel.update(0.1f, Vec2(300.0f, 20.0f), dungeon);
    TEST_ASSERT(!skel.isDead(), "Still alive");

    // Player nearby -> chases and turns towards player
    skel.update(0.1f, Vec2(160.0f, 180.0f), dungeon);
    TEST_ASSERT(skel.getFacing() == Direction::Down, "Chasing down faces Down");

    // Apply sword strike with knockback
    Vec2 kb(0.0f, -120.0f);
    bool hit = skel.takeDamage(1, kb);
    TEST_ASSERT(hit, "Skeleton takes damage");
    TEST_ASSERT(skel.getHP() == 2, "Skeleton HP reduces to 2");
    TEST_ASSERT(skel.isHurt(), "Skeleton is hurt");

    // Knockback update
    float prevY = skel.getPosition().y;
    skel.update(0.05f, Vec2(160.0f, 180.0f), dungeon);
    TEST_ASSERT(skel.getPosition().y < prevY, "Knockback moves skeleton upwards away from strike");

    // Finish off skeleton (advance past hurt timer)
    skel.update(0.35f, Vec2(160.0f, 180.0f), dungeon);
    TEST_ASSERT(!skel.isHurt(), "Skeleton is no longer hurt");
    skel.takeDamage(1, Vec2(0.0f, 0.0f));
    TEST_ASSERT(skel.getHP() == 1, "Skeleton HP is 1");

    skel.update(0.35f, Vec2(160.0f, 180.0f), dungeon);
    skel.takeDamage(1, Vec2(0.0f, 0.0f));
    TEST_ASSERT(skel.isDead(), "Skeleton dies after taking 3 total damage");

    testsPassed++;
}

void test_monster_arena_lock_and_key_unlock() {
    Dungeon dungeon;
    dungeon.init();

    // 1. Enter Room 4 (Monster Arena)
    dungeon.setCurrentRoomId(4);
    auto& r4 = dungeon.getCurrentRoom();
    TEST_ASSERT(r4.isArena, "Room 4 must be flagged as an arena");
    TEST_ASSERT(!r4.arenaCleared, "Room 4 arena is not initially cleared");
    TEST_ASSERT(!r4.arenaLocked, "Room 4 arena is not initially locked");

    // Player entry locks the doors
    bool locked = dungeon.checkArenaEntryLock();
    TEST_ASSERT(locked, "Entry lock triggers upon entering arena");
    TEST_ASSERT(r4.arenaLocked, "Room 4 is now locked");
    TEST_ASSERT(r4.getDoor(Direction::Down) == DoorState::Locked, "South door is locked");
    TEST_ASSERT(r4.getDoor(Direction::Right) == DoorState::Locked, "East door is locked");

    // Calling again returns false (already locked)
    TEST_ASSERT(!dungeon.checkArenaEntryLock(), "Already locked cannot lock again");

    // 2. Clear arena (all monsters defeated)
    bool cleared = dungeon.clearArena();
    TEST_ASSERT(cleared, "Arena unlocks when cleared");
    TEST_ASSERT(!r4.arenaLocked, "Room 4 is no longer locked");
    TEST_ASSERT(r4.arenaCleared, "Room 4 is marked cleared");
    TEST_ASSERT(r4.getDoor(Direction::Down) == DoorState::Open, "South door opens");
    TEST_ASSERT(r4.getDoor(Direction::Right) == DoorState::Open, "East door opens");

    // 3. Test SmallKey ItemDrop persistence & collection
    ItemDrop keyDrop;
    keyDrop.pos = Vec2(160.0f, 120.0f);
    keyDrop.type = ItemType::SmallKey;
    keyDrop.active = true;
    keyDrop.lifetime = 10.0f;
    keyDrop.update(25.0f);
    TEST_ASSERT(keyDrop.active, "SmallKey must never despawn over time");
    TEST_ASSERT(keyDrop.getTextureName() == "small_key", "Key has small_key texture");

    Inventory inventory;
    TEST_ASSERT(inventory.keys == 0, "Initial keys is 0");
    inventory.addKeys(1);
    TEST_ASSERT(inventory.keys == 1, "Keys count is 1 after pickup");

    // 4. Test Room 5 Boss Gate unlocking
    dungeon.setCurrentRoomId(5);
    auto& r5 = dungeon.getCurrentRoom();
    TEST_ASSERT(r5.getDoor(Direction::Up) == DoorState::Locked, "Room 5 North door starts locked");

    // Out of range unlock fails
    TEST_ASSERT(!dungeon.tryUnlockDoor(Vec2(50.0f, 100.0f), Direction::Up, inventory), "Cannot unlock from far away");
    TEST_ASSERT(inventory.keys == 1, "Key not consumed on failed unlock");

    // In front of locked North door facing Up
    Vec2 doorPos(160.0f, 25.0f);
    bool unlocked = dungeon.tryUnlockDoor(doorPos, Direction::Up, inventory);
    TEST_ASSERT(unlocked, "Key successfully unlocks North door");
    TEST_ASSERT(inventory.keys == 0, "Key is consumed");
    TEST_ASSERT(r5.getDoor(Direction::Up) == DoorState::Open, "Room 5 North door is now Open");

    // Connected Room 6 South door is also Open
    const auto* r6 = dungeon.getRoom(6);
    TEST_ASSERT(r6 != nullptr, "Room 6 exists");
    TEST_ASSERT(r6->getDoor(Direction::Down) == DoorState::Open, "Room 6 South door opened from other side");

    // Trying again without keys or already open door returns false
    TEST_ASSERT(!dungeon.tryUnlockDoor(doorPos, Direction::Up, inventory), "Cannot unlock already open door");

    testsPassed++;
}

void test_boss_golem_phases_and_combat() {
    Dungeon dungeon;
    dungeon.init();
    dungeon.setCurrentRoomId(6);

    BossGolemStateMachine golem(144.0f, 56.0f);
    TEST_ASSERT(golem.getPhase() == BossPhase::ArmoredStomp, "Golem starts in Phase 1: ArmoredStomp");
    TEST_ASSERT(golem.getHP() == 12 && golem.getMaxHP() == 12, "Golem starts with 12 HP");
    TEST_ASSERT(golem.isArmored(), "Golem is armored in Phase 1");
    TEST_ASSERT(golem.getTextureName() == "boss_golem_armored", "Armored texture in Phase 1");

    Vec2 playerPos(144.0f, 180.0f);

    // Phase 1: Sword and arrows bounce off armor
    Rect swordBox = golem.getBodyHitbox();
    Rect arrowBox = golem.getEyeHitbox();
    TEST_ASSERT(!golem.trySwordHit(swordBox, 1), "Sword bounces off armored golem in Phase 1");
    TEST_ASSERT(!golem.tryArrowHit(arrowBox, 1), "Arrow bounces off armored eye in Phase 1");
    TEST_ASSERT(golem.getHP() == 12, "HP unchanged after deflected hits");

    // Golem stomps toward player and hurls a rock after 1.4s
    float startY = golem.getPosition().y;
    golem.update(1.5f, playerPos, dungeon);
    TEST_ASSERT(golem.getPosition().y > startY, "Golem stomps downward toward player");
    TEST_ASSERT(golem.consumeRockFiredEvent(), "Golem hurls rock projectile");
    TEST_ASSERT(!golem.getRocks().empty(), "Active rock projectile spawned");

    // Advance remaining 2.6s (total 4.1s >= 4.0s) to enter Phase 2: EyeCharging
    golem.update(2.6f, playerPos, dungeon);
    TEST_ASSERT(golem.getPhase() == BossPhase::EyeCharging, "Golem enters Phase 2: EyeCharging");
    TEST_ASSERT(golem.getTextureName() == "boss_golem_charging", "Charging texture in Phase 2");
    TEST_ASSERT(!golem.trySwordHit(golem.getBodyHitbox(), 1), "Sword still bounces off in Phase 2");

    // Hitting the charging eye with an arrow stuns the golem for 3.5 seconds!
    bool stunned = golem.tryArrowHit(golem.getEyeHitbox(), 1);
    TEST_ASSERT(stunned, "Arrow hitting charging eye stuns the golem");
    TEST_ASSERT(golem.getPhase() == BossPhase::Stunned, "Golem enters Phase 3: Stunned");
    TEST_ASSERT(golem.getHP() == 11, "Arrow stun deals 1 damage");
    TEST_ASSERT(std::abs(golem.getStunTimeRemaining() - 3.5f) < 0.001f, "Stun duration is 3.5 seconds");
    TEST_ASSERT(golem.getTextureName() == "boss_golem_stunned", "Stunned texture in Phase 3");

    // Phase 3: Stunned golem takes full sword combo damage
    golem.update(0.3f, playerPos, dungeon); // Clear hurt timer from arrow hit
    float initialSpeed = golem.getSpeed();

    for (int i = 0; i < 4; ++i) {
        bool swordHit = golem.trySwordHit(golem.getBodyHitbox(), 1);
        TEST_ASSERT(swordHit, "Stunned golem takes sword damage");
        golem.update(0.25f, playerPos, dungeon);
    }

    // After 4 hits (damage cap), golem roars, speeds up, and returns to ArmoredStomp
    TEST_ASSERT(golem.getHP() == 7, "Golem HP is 7 after 4 sword combo hits");
    TEST_ASSERT(golem.getPhase() == BossPhase::ArmoredStomp, "Golem recovers after max stun hits");
    TEST_ASSERT(golem.consumeRoarEvent(), "Golem roars upon recovering from stun");
    TEST_ASSERT(golem.getSpeed() > initialSpeed, "Golem speeds up after stun cycle");

    // Also test stun timer expiration (3.5s without hitting damage cap)
    golem.setPhase(BossPhase::EyeCharging);
    TEST_ASSERT(golem.tryArrowHit(golem.getEyeHitbox(), 1), "Arrow stuns again (HP -> 6)");
    TEST_ASSERT(golem.getPhase() == BossPhase::Stunned, "Stunned again");
    golem.update(3.55f, playerPos, dungeon);
    TEST_ASSERT(golem.getPhase() == BossPhase::ArmoredStomp, "Stun expires after 3.5 seconds");

    // Deal lethal damage to trigger Dying -> Defeated sequence
    golem.setPhase(BossPhase::Stunned);
    bool killed = golem.trySwordHit(golem.getBodyHitbox(), 6);
    TEST_ASSERT(killed, "Lethal sword strike lands");
    TEST_ASSERT(golem.getHP() == 0 && golem.isDead(), "Golem HP is 0 and dead");
    TEST_ASSERT(golem.isDying(), "Golem enters Dying phase for explosion sequence");
    TEST_ASSERT(!golem.getExplosions().empty(), "Explosion particles spawned on death");

    // Advance past 2.0s dying sequence
    golem.update(2.1f, playerPos, dungeon);
    TEST_ASSERT(golem.isDefeated(), "Golem transitions to Defeated after death sequence");

    // Heart Container drop and +1 Max Heart reward
    ItemDrop hcDrop;
    hcDrop.pos = Vec2(152.0f, 104.0f);
    hcDrop.type = ItemType::HeartContainer;
    hcDrop.active = true;
    hcDrop.update(30.0f);
    TEST_ASSERT(hcDrop.active, "HeartContainer drop never despawns");
    TEST_ASSERT(hcDrop.getTextureName() == "heart_container", "HeartContainer texture name");

    Player hero;
    hero.takeDamage(2);
    TEST_ASSERT(hero.getHealth().getMaxHearts() == 3 && hero.getHealth().getCurrentHP() == 4, "Hero at 2/3 hearts");
    hero.getHealth().addHeartContainer();
    TEST_ASSERT(hero.getHealth().getMaxHearts() == 4 && hero.getHealth().getCurrentHP() == 8, "HeartContainer grants +1 max heart and full heal");

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
    test_dungeon_room_layout_and_plate();
    test_dungeon_room_transitions();
    test_arrow_projectile();
    test_treasure_chest_in_room_3();
    test_crystal_switch_arrow_trigger();
    test_bat_movement_and_damage();
    test_skeleton_patrol_chase_and_knockback();
    test_monster_arena_lock_and_key_unlock();
    test_boss_golem_phases_and_combat();

    std::cout << "Tests Passed: " << testsPassed << " | Tests Failed: " << testsFailed << std::endl;
    return testsFailed == 0 ? 0 : 1;
}
