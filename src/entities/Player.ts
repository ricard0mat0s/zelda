import Phaser from 'phaser';
import { Direction } from '../types/game';
import { computeMovementVelocity, MovementInput } from './movement';

export class Player extends Phaser.Physics.Arcade.Sprite {
  public facing: Direction = 'down';
  public speed: number = 90;

  private cursors?: Phaser.Types.Input.Keyboard.CursorKeys;
  private wasd?: {
    up: Phaser.Input.Keyboard.Key;
    down: Phaser.Input.Keyboard.Key;
    left: Phaser.Input.Keyboard.Key;
    right: Phaser.Input.Keyboard.Key;
  };

  constructor(scene: Phaser.Scene, x: number, y: number) {
    super(scene, x, y, 'hero_down_0');

    scene.add.existing(this);
    scene.physics.add.existing(this);

    // Precise retro hitbox clamped around the character's feet for natural depth sorting
    this.setSize(12, 10);
    this.setOffset(2, 6);
    this.setCollideWorldBounds(true);

    this.initAnimations(scene);
    this.initControls(scene);
  }

  private initAnimations(scene: Phaser.Scene): void {
    const anims = scene.anims;
    const directions: Direction[] = ['down', 'up', 'left', 'right'];

    for (const dir of directions) {
      const animKey = `hero_walk_${dir}`;
      if (!anims.exists(animKey)) {
        anims.create({
          key: animKey,
          frames: [
            { key: `hero_${dir}_0` },
            { key: `hero_${dir}_1` },
          ],
          frameRate: 6,
          repeat: -1,
        });
      }
    }
  }

  private initControls(scene: Phaser.Scene): void {
    if (!scene.input.keyboard) return;

    this.cursors = scene.input.keyboard.createCursorKeys();
    this.wasd = {
      up: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.W),
      down: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.S),
      left: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.A),
      right: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.D),
    };
  }

  update(): void {
    const input: MovementInput = {
      up: Boolean(this.cursors?.up?.isDown || this.wasd?.up?.isDown),
      down: Boolean(this.cursors?.down?.isDown || this.wasd?.down?.isDown),
      left: Boolean(this.cursors?.left?.isDown || this.wasd?.left?.isDown),
      right: Boolean(this.cursors?.right?.isDown || this.wasd?.right?.isDown),
    };

    const { vx, vy, facing, isMoving } = computeMovementVelocity(input, this.facing, this.speed);
    this.facing = facing;
    this.setVelocity(vx, vy);

    if (isMoving) {
      this.play(`hero_walk_${facing}`, true);
    } else {
      this.stop();
      this.setTexture(`hero_${facing}_0`);
    }
  }
}
