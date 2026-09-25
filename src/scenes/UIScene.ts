import Phaser from 'phaser';
import { HealthSystem } from '../systems/HealthSystem';
import { HeartDisplayUnit } from '../types/game';
import { createRetroUITextStyle } from '../utils/text';

export interface UISceneData {
  health: HealthSystem;
}

export class UIScene extends Phaser.Scene {
  private health?: HealthSystem;
  private heartImages: Phaser.GameObjects.Image[] = [];

  constructor() {
    super('UIScene');
  }

  create(data?: UISceneData): void {
    this.heartImages = [];

    // HUD Header: "- LIFE -"
    this.add.text(12, 8, '- LIFE -', createRetroUITextStyle({
      fontSize: '8px',
      color: '#e53e3e',
      stroke: '#0b0c10',
      strokeThickness: 2,
    })).setOrigin(0, 0);

    if (data?.health) {
      this.bindHealthSystem(data.health);
    }
  }

  public bindHealthSystem(health: HealthSystem): void {
    this.health = health;

    // Initial render
    this.renderHearts(health.getHeartUnits());

    // Re-render on any health change
    health.onHealthChanged((stats) => {
      this.renderHearts(stats.units);
    });
  }

  public getHealth(): HealthSystem | undefined {
    return this.health;
  }

  private renderHearts(units: HeartDisplayUnit[]): void {
    const startX = 14;
    const startY = 24;
    const spacing = 15;

    // Expand image array if max hearts grew
    while (this.heartImages.length < units.length) {
      const idx = this.heartImages.length;
      const img = this.add.image(startX + idx * spacing, startY, 'heart_full');
      img.setOrigin(0, 0);
      this.heartImages.push(img);
    }

    // Shrink if max hearts reduced
    while (this.heartImages.length > units.length) {
      const img = this.heartImages.pop();
      img?.destroy();
    }

    // Update textures
    for (let i = 0; i < units.length; i++) {
      const unit = units[i];
      const textureKey = `heart_${unit.type}`;
      this.heartImages[i].setTexture(textureKey);
    }
  }
}
