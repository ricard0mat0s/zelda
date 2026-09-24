import Phaser from 'phaser';
import { createPhaserConfig } from './config';
import { BootScene } from './scenes/BootScene';
import { TitleScene } from './scenes/TitleScene';
import { OverworldScene } from './scenes/OverworldScene';

window.addEventListener('DOMContentLoaded', () => {
  const config = createPhaserConfig([BootScene, TitleScene, OverworldScene]);
  new Phaser.Game(config);
});
