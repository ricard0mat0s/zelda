import Phaser from 'phaser';

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
    this.add.text(cx, 115, 'THE LEGEND OF', {
      fontFamily: 'monospace',
      fontSize: '10px',
      color: '#a0aec0',
      align: 'center',
    }).setOrigin(0.5);

    // Title Text: "HERO" (Bold golden retro title)
    const titleText = this.add.text(cx, 132, 'ARCADE HERO', {
      fontFamily: 'monospace',
      fontSize: '20px',
      fontStyle: 'bold',
      color: '#ffd700',
      align: 'center',
      stroke: '#b7791f',
      strokeThickness: 2,
    }).setOrigin(0.5);

    // Subtle floating animation for title
    this.tweens.add({
      targets: titleText,
      y: 130,
      duration: 1200,
      yoyo: true,
      repeat: -1,
      ease: 'Sine.easeInOut',
    });

    // Subtitle
    this.add.text(cx, 150, '16-BIT RETRO ADVENTURE', {
      fontFamily: 'monospace',
      fontSize: '8px',
      color: '#63b3ed',
      align: 'center',
    }).setOrigin(0.5);

    // Prompt: "PRESS SPACE TO START" with retro blinking tween
    const promptText = this.add.text(cx, 185, 'PRESS SPACE TO START', {
      fontFamily: 'monospace',
      fontSize: '11px',
      fontStyle: 'bold',
      color: '#ffffff',
      align: 'center',
    }).setOrigin(0.5);

    this.tweens.add({
      targets: promptText,
      alpha: 0.1,
      duration: 500,
      yoyo: true,
      repeat: -1,
      ease: 'Stepped',
    });

    // Footer / Controls Hint
    this.add.text(cx, 222, 'CONTROLS: WASD / ARROWS = MOVE | SPACE = ATTACK', {
      fontFamily: 'monospace',
      fontSize: '7px',
      color: '#718096',
      align: 'center',
    }).setOrigin(0.5);

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
    this.cameras.main.fade(300, 0, 0, 0, false, (_camera: Phaser.Cameras.Scene2D.Camera, progress: number) => {
      if (progress === 1) {
        this.scene.start('OverworldScene');
      }
    });
  }
}
