import { Direction } from '../types/game';

export interface HitboxRect {
  x: number;
  y: number;
  width: number;
  height: number;
}

export interface KnockbackVector {
  vx: number;
  vy: number;
}

/**
 * Calculates sword slash hitbox in front of player based on 4-directional facing.
 */
export function getSwordHitbox(playerPos: { x: number; y: number }, facing: Direction): HitboxRect {
  switch (facing) {
    case 'down':
      return {
        x: playerPos.x - 8,
        y: playerPos.y + 6,
        width: 16,
        height: 14,
      };
    case 'up':
      return {
        x: playerPos.x - 8,
        y: playerPos.y - 18,
        width: 16,
        height: 14,
      };
    case 'left':
      return {
        x: playerPos.x - 18,
        y: playerPos.y - 8,
        width: 14,
        height: 16,
      };
    case 'right':
      return {
        x: playerPos.x + 4,
        y: playerPos.y - 8,
        width: 14,
        height: 16,
      };
  }
}

/**
 * Calculates knockback vector away from attacker with normalized force.
 */
export function calculateKnockback(
  attackerPos: { x: number; y: number },
  targetPos: { x: number; y: number },
  force = 140
): KnockbackVector {
  const dx = targetPos.x - attackerPos.x;
  const dy = targetPos.y - attackerPos.y;
  const dist = Math.hypot(dx, dy);

  if (dist === 0) {
    // Default downwards impulse if precisely overlapping
    return { vx: 0, vy: force };
  }

  return {
    vx: (dx / dist) * force,
    vy: (dy / dist) * force,
  };
}

/**
 * Validates if an attack action can be triggered.
 */
export function canPerformAttack(
  hasWeapon: boolean,
  isAttacking: boolean,
  cooldownRemaining: number
): boolean {
  return hasWeapon && !isAttacking && cooldownRemaining <= 0;
}

/**
 * Pure domain inventory manager for weapons and keys.
 */
export class InventorySystem {
  public hasSword: boolean = false;
  public hasBow: boolean = false;
  public keys: number = 0;

  public obtainSword(): void {
    this.hasSword = true;
  }

  public obtainBow(): void {
    this.hasBow = true;
  }

  public addKeys(amount: number = 1): void {
    if (amount > 0) {
      this.keys += amount;
    }
  }

  public useKey(): boolean {
    if (this.keys > 0) {
      this.keys--;
      return true;
    }
    return false;
  }
}
