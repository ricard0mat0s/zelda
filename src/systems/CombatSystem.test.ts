import { describe, it, expect } from 'vitest';
import {
  getSwordHitbox,
  calculateKnockback,
  canPerformAttack,
  InventorySystem,
} from './CombatSystem';

describe('Combat System & Inventory (Pure Domain Logic)', () => {
  it('positions sword attack hitboxes correctly based on 4-directional facing', () => {
    const playerPos = { x: 100, y: 100 };

    const downHitbox = getSwordHitbox(playerPos, 'down');
    expect(downHitbox.y).toBeGreaterThan(playerPos.y);
    expect(downHitbox.width).toBeGreaterThanOrEqual(14);
    expect(downHitbox.height).toBeGreaterThanOrEqual(12);

    const upHitbox = getSwordHitbox(playerPos, 'up');
    expect(upHitbox.y).toBeLessThan(playerPos.y);

    const leftHitbox = getSwordHitbox(playerPos, 'left');
    expect(leftHitbox.x).toBeLessThan(playerPos.x);

    const rightHitbox = getSwordHitbox(playerPos, 'right');
    expect(rightHitbox.x).toBeGreaterThan(playerPos.x);
  });

  it('calculates radial knockback vectors away from the attacker', () => {
    const attacker = { x: 100, y: 100 };
    const targetRight = { x: 110, y: 100 };

    const kbRight = calculateKnockback(attacker, targetRight, 100);
    expect(kbRight.vx).toBeCloseTo(100, 1);
    expect(kbRight.vy).toBeCloseTo(0, 1);

    const targetUpLeft = { x: 90, y: 90 };
    const kbUpLeft = calculateKnockback(attacker, targetUpLeft, 100);
    expect(kbUpLeft.vx).toBeLessThan(0);
    expect(kbUpLeft.vy).toBeLessThan(0);
    expect(Math.hypot(kbUpLeft.vx, kbUpLeft.vy)).toBeCloseTo(100, 1);
  });

  it('restricts attack execution when unarmed or during cooldown', () => {
    // Unarmed cannot attack
    expect(canPerformAttack(false, false, 0)).toBe(false);

    // Armed can attack when ready
    expect(canPerformAttack(true, false, 0)).toBe(true);

    // Cannot attack while already attacking
    expect(canPerformAttack(true, true, 0)).toBe(false);

    // Cannot attack while cooldown is active
    expect(canPerformAttack(true, false, 150)).toBe(false);
  });

  it('manages weapon acquisition and key inventory transitions', () => {
    const inv = new InventorySystem();
    expect(inv.hasSword).toBe(false);
    expect(inv.hasBow).toBe(false);
    expect(inv.keys).toBe(0);

    inv.obtainSword();
    expect(inv.hasSword).toBe(true);

    inv.addKeys(2);
    expect(inv.keys).toBe(2);

    expect(inv.useKey()).toBe(true);
    expect(inv.keys).toBe(1);

    expect(inv.useKey()).toBe(true);
    expect(inv.keys).toBe(0);

    // Cannot use key when 0 remaining
    expect(inv.useKey()).toBe(false);
    expect(inv.keys).toBe(0);
  });
});
