import { describe, it, expect } from 'vitest';
import { MAP_CONFIG } from './map';

describe('Map Dimensions & Scaling', () => {
  it('defines 40x30 tile dimensions with 16px tiles (640x480 world size)', () => {
    expect(MAP_CONFIG.COLS).toBe(40);
    expect(MAP_CONFIG.ROWS).toBe(30);
    expect(MAP_CONFIG.TILE_SIZE).toBe(16);
    expect(MAP_CONFIG.WORLD_WIDTH).toBe(640);
    expect(MAP_CONFIG.WORLD_HEIGHT).toBe(480);
  });
});
