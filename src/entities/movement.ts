import { Direction } from '../types/game';

export interface MovementInput {
  up: boolean;
  down: boolean;
  left: boolean;
  right: boolean;
}

export interface MovementResult {
  vx: number;
  vy: number;
  facing: Direction;
  isMoving: boolean;
}

/**
 * Computes velocity and facing direction from directional input.
 * Normalizes diagonal movement so diagonal speed matches cardinal speed.
 * Preserves facing direction when coming to a halt.
 */
export function computeMovementVelocity(
  input: MovementInput,
  currentFacing: Direction,
  speed: number
): MovementResult {
  let dx = 0;
  let dy = 0;

  if (input.left && !input.right) dx = -1;
  else if (input.right && !input.left) dx = 1;

  if (input.up && !input.down) dy = -1;
  else if (input.down && !input.up) dy = 1;

  const isMoving = dx !== 0 || dy !== 0;

  let facing = currentFacing;
  if (isMoving) {
    if (dy > 0) facing = 'down';
    else if (dy < 0) facing = 'up';
    else if (dx > 0) facing = 'right';
    else if (dx < 0) facing = 'left';
  }

  let vx = 0;
  let vy = 0;

  if (isMoving) {
    if (dx !== 0 && dy !== 0) {
      // Normalize diagonal speed: sqrt(1/2) ≈ 0.7071
      const factor = Math.SQRT1_2;
      vx = dx * speed * factor;
      vy = dy * speed * factor;
    } else {
      vx = dx * speed;
      vy = dy * speed;
    }
  }

  return { vx, vy, facing, isMoving };
}
