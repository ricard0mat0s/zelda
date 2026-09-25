export const MAP_CONFIG = {
  COLS: 40,
  ROWS: 30,
  TILE_SIZE: 16,
  get WORLD_WIDTH(): number {
    return this.COLS * this.TILE_SIZE;
  },
  get WORLD_HEIGHT(): number {
    return this.ROWS * this.TILE_SIZE;
  },
} as const;
