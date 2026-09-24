import Phaser from 'phaser';
import { createRetroTextStyle } from '../utils/text';
import { AudioSystem } from '../systems/AudioSystem';

export class TitleScene extends Phaser.Scene {
  private spaceKey?: Phaser.Input.Keyboard.Key;
  private isStarting: boolean = false;

  constructor() {
    super('TitleScene');
  }

  create(): void {
    this.isStarting = false;
    const { width, height } = this.cameras.main;
    const cx = width / 2;

    // Background decoration - dark retro slate with star/particle motes
    const bgGraphics = this.add.graphics();
    bgGraphics.fillStyle(0x0a0c10, 1);
    bgGraphics.fillRect(0, 0, width, height);

    // Decorative Triforce Symbol (Golden Triangles)
    const triGraphics = this.add.graphics();
    triGraphics.fillStyle(0xffd700, 1);
    
    const triSize = 20;
    const triY = 48;
    // Top triangle
    triGraphics.fillTriangle(cx, triY, cx - triSize, triY + triSize * 1.5, cx + triSize, triY + triSize * 1.5);
    // Bottom-left triangle
    triGraphics.fillTriangle(cx - triSize, triY + triSize * 1.5, cx - triSize * 2, triY + triSize * 3, cx, triY + triSize * 3);
    // Bottom-right triangle
    triGraphics.fillTriangle(cx + triSize, triY + triSize * 1.5, cx, triY + triSize * 3, cx + triSize * 2, triY + triSize * 3);

    // Title Text: "THE LEGEND OF"
    this.add.text(cx, 114, 'THE LEGEND OF', createRetroTextStyle({
      fontSize: '8px',
      color: '#a0aec0',
      align: 'center',
    })).setOrigin(0.5);

    // Title Text: "ARCADE HERO" (Bold golden retro title - static, no floating animation)
    this.add.text(cx, 130, 'ARCADE HERO', createRetroTextStyle({
      fontSize: '15px',
      color: '#ffd700',
      align: 'center',
      stroke: '#744210',
      strokeThickness: 3,
    })).setOrigin(0.5);

    // Subtitle
    this.add.text(cx, 152, '16-BIT RETRO ADVENTURE', createRetroTextStyle({
      fontSize: '8px',
      color: '#90cdf4',
      align: 'center',
    })).setOrigin(0.5);

    // Prompt: "PRESS SPACE TO START" - appears for a long time (1200ms) and blinks off for a short time (200ms)
    const promptText = this.add.text(cx, 185, 'PRESS SPACE TO START', createRetroTextStyle({
      fontSize: '8px',
      color: '#ffffff',
      align: 'center',
    })).setOrigin(0.5);

    this.time.addEvent({
      delay: 1400,
      loop: true,
      callback: () => {
        promptText.setVisible(false);
        this.time.delayedCall(50, () => {
          promptText.setVisible(true);
        });
      },
    });

    // Footer / Controls Hint - Increased to 8px for crisp pixel-grid alignment without anti-aliasing
    this.add.text(cx, 220, 'WASD/ARROWS: MOVE | SPACE: ATTACK', createRetroTextStyle({
      fontSize: '8px',
      color: '#ffffff',
      align: 'center',
    })).setOrigin(0.5);

    // Keyboard Input
    if (this.input.keyboard) {
      this.spaceKey = this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SPACE);
    }
  }

  update(): void {
    if (this.spaceKey && Phaser.Input.Keyboard.JustDown(this.spaceKey) && !this.isStarting) {
      this.startGame();
    }
  }

  private startGame(): void {
    this.isStarting = true;
    AudioSystem.getInstance().playGameStart();
    this.cameras.main.fade(300, 0, 0, 0, false, (_camera: Phaser.Cameras.Scene2D.Camera, progress: number) => {
      if (progress === 1) {
        this.scene.start('OverworldScene');
      }
    });
  }
}
