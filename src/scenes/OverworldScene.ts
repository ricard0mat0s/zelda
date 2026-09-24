import Phaser from 'phaser';

export class OverworldScene extends Phaser.Scene {
  constructor() {
    super('OverworldScene');
  }

  create(): void {
    const { width, height } = this.cameras.main;
    const cx = width / 2;
    const cy = height / 2;

    // Green grassy glade background
    const bg = this.add.graphics();
    bg.fillStyle(0x2d6a4f, 1);
    bg.fillRect(0, 0, width, height);

    // Decorative pathway
    bg.fillStyle(0xd4a373, 1);
    bg.fillRect(cx - 16, 0, 32, height);

    this.add.text(cx, cy - 20, 'OVERWORLD GROVE', {
      fontFamily: 'monospace',
      fontSize: '14px',
      fontStyle: 'bold',
      color: '#ffffff',
      stroke: '#081c15',
      strokeThickness: 3,
    }).setOrigin(0.5);

    this.add.text(cx, cy + 10, 'Ready for Phase 2: Player & Tilemap', {
      fontFamily: 'monospace',
      fontSize: '8px',
      color: '#d8f3dc',
    }).setOrigin(0.5);
  }
}
