import { describe, it, expect, beforeEach, vi } from 'vitest';
import { HealthSystem } from './HealthSystem';

describe('HealthSystem (Pure Domain Logic)', () => {
  let health: HealthSystem;

  beforeEach(() => {
    health = new HealthSystem(3); // 3 hearts = 6 HP
  });

  it('initializes with correct hearts and full HP units', () => {
    expect(health.getMaxHearts()).toBe(3);
    expect(health.getMaxHP()).toBe(6);
    expect(health.getCurrentHP()).toBe(6);
    expect(health.isDead()).toBe(false);
    expect(health.getHeartUnits()).toEqual([
      { type: 'full' },
      { type: 'full' },
      { type: 'full' },
    ]);
  });

  it('applies damage and returns half-heart display status', () => {
    const res = health.takeDamage(1);
    expect(res.damaged).toBe(true);
    expect(res.isDead).toBe(false);
    expect(res.remainingHP).toBe(5);
    expect(health.getCurrentHP()).toBe(5);
    expect(health.getHeartUnits()).toEqual([
      { type: 'full' },
      { type: 'full' },
      { type: 'half' },
    ]);
  });

  it('activates invulnerability and blocks subsequent damage until cooldown expires', () => {
    health.takeDamage(1);
    expect(health.isInvulnerable()).toBe(true);

    // Immediate second hit is blocked by invulnerability
    const blockedRes = health.takeDamage(2);
    expect(blockedRes.damaged).toBe(false);
    expect(health.getCurrentHP()).toBe(5);

    // Advance 600ms of the 1000ms cooldown
    health.update(600);
    expect(health.isInvulnerable()).toBe(true);
    expect(health.takeDamage(1).damaged).toBe(false);

    // Advance remaining 400ms -> cooldown expires
    health.update(400);
    expect(health.isInvulnerable()).toBe(false);

    // Damage now registers
    const secondHit = health.takeDamage(2);
    expect(secondHit.damaged).toBe(true);
    expect(health.getCurrentHP()).toBe(3);
    expect(health.getHeartUnits()).toEqual([
      { type: 'full' },
      { type: 'half' },
      { type: 'empty' },
    ]);
  });

  it('heals current HP without exceeding max HP cap', () => {
    health.takeDamage(3);
    health.update(1000); // clear invuln
    expect(health.getCurrentHP()).toBe(3);

    health.heal(2);
    expect(health.getCurrentHP()).toBe(5);

    health.heal(10); // Overheal is capped at max HP (6)
    expect(health.getCurrentHP()).toBe(6);
    expect(health.getHeartUnits()).toEqual([
      { type: 'full' },
      { type: 'full' },
      { type: 'full' },
    ]);
  });

  it('triggers death state and onDied callback on fatal damage', () => {
    const onDiedCallback = vi.fn();
    health.onDied(onDiedCallback);

    const fatalHit = health.takeDamage(6);
    expect(fatalHit.damaged).toBe(true);
    expect(fatalHit.isDead).toBe(true);
    expect(fatalHit.remainingHP).toBe(0);
    expect(health.isDead()).toBe(true);
    expect(onDiedCallback).toHaveBeenCalledTimes(1);

    expect(health.getHeartUnits()).toEqual([
      { type: 'empty' },
      { type: 'empty' },
      { type: 'empty' },
    ]);

    // Dead entity cannot heal
    health.heal(4);
    expect(health.getCurrentHP()).toBe(0);
  });

  it('increases max capacity when adding a Heart Container', () => {
    health.takeDamage(2);
    health.addHeartContainer();

    expect(health.getMaxHearts()).toBe(4);
    expect(health.getMaxHP()).toBe(8);
    expect(health.getCurrentHP()).toBe(8); // Full restore
    expect(health.getHeartUnits()).toEqual([
      { type: 'full' },
      { type: 'full' },
      { type: 'full' },
      { type: 'full' },
    ]);
  });
});
