import Phaser from 'phaser';
import { createPhaserConfig } from './config';
import { BootScene } from './scenes/BootScene';
import { TitleScene } from './scenes/TitleScene';
import { OverworldScene } from './scenes/OverworldScene';
import { UIScene } from './scenes/UIScene';

window.addEventListener('DOMContentLoaded', async () => {
  if (document.fonts) {
    try {
      await document.fonts.ready;
    } catch {
      // Fallback gracefully if fonts fail to load
    }
  }
  const config = createPhaserConfig([BootScene, TitleScene, OverworldScene, UIScene]);
  new Phaser.Game(config);
});

