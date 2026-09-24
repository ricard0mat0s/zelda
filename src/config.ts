import type Phaser from 'phaser';

export const GAME_SETTINGS = {
  TITLE: 'Zelda: Retro Adventure',
  WIDTH: 320,
  HEIGHT: 240,
  TILE_SIZE: 16,
  SCALE_MODE_FIT: 3, // Phaser.Scale.FIT
  SCALE_CENTER_BOTH: 1, // Phaser.Scale.CENTER_BOTH
} as const;

export function createPhaserConfig(scenes: Phaser.Types.Scenes.SceneType[]): Phaser.Types.Core.GameConfig {
  return {
    type: 0, // Phaser.AUTO
    title: GAME_SETTINGS.TITLE,
    parent: 'game-container',
    width: GAME_SETTINGS.WIDTH,
    height: GAME_SETTINGS.HEIGHT,
    pixelArt: true,
    roundPixels: true,
    scale: {
      mode: GAME_SETTINGS.SCALE_MODE_FIT,
      autoCenter: GAME_SETTINGS.SCALE_CENTER_BOTH,
      width: GAME_SETTINGS.WIDTH,
      height: GAME_SETTINGS.HEIGHT,
    },
    physics: {
      default: 'arcade',
      arcade: {
        gravity: { x: 0, y: 0 },
        debug: false,
      },
    },
    scene: scenes,
    backgroundColor: '#0b0c10',
  };
}
