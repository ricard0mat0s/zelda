import { describe, it, expect } from 'vitest';
import { computeMovementVelocity } from './movement';

describe('Player Movement Logic', () => {
  const SPEED = 100;

  it('calculates cardinal velocities and updates facing direction', () => {
    const up = computeMovementVelocity({ up: true, down: false, left: false, right: false }, 'down', SPEED);
    expect(up.vx).toBe(0);
    expect(up.vy).toBe(-SPEED);
    expect(up.facing).toBe('up');
    expect(up.isMoving).toBe(true);

    const down = computeMovementVelocity({ up: false, down: true, left: false, right: false }, 'up', SPEED);
    expect(down.vx).toBe(0);
    expect(down.vy).toBe(SPEED);
    expect(down.facing).toBe('down');
    expect(down.isMoving).toBe(true);

    const left = computeMovementVelocity({ up: false, down: false, left: true, right: false }, 'down', SPEED);
    expect(left.vx).toBe(-SPEED);
    expect(left.vy).toBe(0);
    expect(left.facing).toBe('left');
    expect(left.isMoving).toBe(true);

    const right = computeMovementVelocity({ up: false, down: false, left: false, right: true }, 'down', SPEED);
    expect(right.vx).toBe(SPEED);
    expect(right.vy).toBe(0);
    expect(right.facing).toBe('right');
    expect(right.isMoving).toBe(true);
  });

  it('normalizes diagonal velocity so speed does not exceed standard max speed', () => {
    const diagonal = computeMovementVelocity({ up: true, down: false, left: false, right: true }, 'down', SPEED);
    const totalSpeed = Math.hypot(diagonal.vx, diagonal.vy);
    expect(totalSpeed).toBeCloseTo(SPEED, 2);
    expect(diagonal.vx).toBeGreaterThan(0);
    expect(diagonal.vy).toBeLessThan(0);
    expect(diagonal.isMoving).toBe(true);
  });

  it('preserves current facing direction and sets zero velocity when idle', () => {
    const idle = computeMovementVelocity({ up: false, down: false, left: false, right: false }, 'right', SPEED);
    expect(idle.vx).toBe(0);
    expect(idle.vy).toBe(0);
    expect(idle.facing).toBe('right');
    expect(idle.isMoving).toBe(false);
  });

  it('cancels out opposing key presses (e.g. left + right, up + down)', () => {
    const cancelled = computeMovementVelocity({ up: true, down: true, left: true, right: true }, 'up', SPEED);
    expect(cancelled.vx).toBe(0);
    expect(cancelled.vy).toBe(0);
    expect(cancelled.isMoving).toBe(false);
  });
});
