import { describe, it, expect } from 'vitest';
import { GAME_SETTINGS, createPhaserConfig } from './config';

describe('Game Configuration', () => {
  it('defines correct retro resolution (320x240) and 16px tile dimensions', () => {
    expect(GAME_SETTINGS.WIDTH).toBe(320);
    expect(GAME_SETTINGS.HEIGHT).toBe(240);
    expect(GAME_SETTINGS.TILE_SIZE).toBe(16);
  });

  it('generates Phaser arcade configuration with pixel art scaling and zero gravity', () => {
    const config = createPhaserConfig([]);
    expect(config.pixelArt).toBe(true);
    expect(config.scale?.mode).toBeDefined();
    expect(config.physics?.default).toBe('arcade');
    expect(config.physics?.arcade?.gravity).toEqual({ x: 0, y: 0 });
  });
});
