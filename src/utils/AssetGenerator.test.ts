import { describe, it, expect } from 'vitest';
import { ASSET_DEFINITIONS } from './AssetGenerator';

describe('AssetGenerator Pixel Definitions', () => {
  it('defines all required tiles as 16x16 pixel grids', () => {
    const tiles = ['grass', 'path', 'tree', 'water', 'stone_wall'] as const;
    for (const key of tiles) {
      const def = ASSET_DEFINITIONS[key];
      expect(def).toBeDefined();
      expect(def.pixels.length).toBe(16);
      for (const row of def.pixels) {
        expect(row.length).toBe(16);
      }
    }
  });

  it('defines all 4-directional Hero animation frames (8 frames total) as 16x16 grids', () => {
    const heroFrames = [
      'hero_down_0', 'hero_down_1',
      'hero_up_0', 'hero_up_1',
      'hero_left_0', 'hero_left_1',
      'hero_right_0', 'hero_right_1',
    ] as const;

    for (const key of heroFrames) {
      const def = ASSET_DEFINITIONS[key];
      expect(def).toBeDefined();
      expect(def.pixels.length).toBe(16);
      for (const row of def.pixels) {
        expect(row.length).toBe(16);
      }
    }
  });

  it('ensures every character in pixel grids maps to a valid palette color hex', () => {
    for (const [key, def] of Object.entries(ASSET_DEFINITIONS)) {
      for (const row of def.pixels) {
        for (const char of row) {
          expect(def.palette[char], `Missing palette color for char "${char}" in asset "${key}"`).toBeDefined();
          expect(def.palette[char]).toMatch(/^#([0-9a-fA-F]{3,8}|00000000)$/);
        }
      }
    }
  });
});
