import Phaser from 'phaser';
import { Direction } from '../types/game';
import { computeMovementVelocity, MovementInput } from './movement';
import { DamageResult, HealthSystem } from '../systems/HealthSystem';
import { AudioSystem } from '../systems/AudioSystem';
import { canPerformAttack, getSwordHitbox, HitboxRect, InventorySystem } from '../systems/CombatSystem';

export class Player extends Phaser.Physics.Arcade.Sprite {
  public facing: Direction = 'down';
  public speed: number = 90;
  public health: HealthSystem = new HealthSystem(3);
  public inventory: InventorySystem = new InventorySystem();
  public isAttacking: boolean = false;
  public canMove: boolean = true;

  private attackTimer: number = 0;
  private attackCooldown: number = 0;
  private slashSprite?: Phaser.GameObjects.Sprite;

  private cursors?: Phaser.Types.Input.Keyboard.CursorKeys;
  private wasd?: {
    up: Phaser.Input.Keyboard.Key;
    down: Phaser.Input.Keyboard.Key;
    left: Phaser.Input.Keyboard.Key;
    right: Phaser.Input.Keyboard.Key;
  };
  private attackKeys?: {
    space: Phaser.Input.Keyboard.Key;
    z: Phaser.Input.Keyboard.Key;
  };
  private debugKeys?: {
    hurt: Phaser.Input.Keyboard.Key;
    heal: Phaser.Input.Keyboard.Key;
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

    // Attack keys: SPACE or Z
    this.attackKeys = {
      space: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SPACE),
      z: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.Z),
    };

    // Debug test keys: H = hurt 1 HP (half-heart), J = heal 1 HP
    this.debugKeys = {
      hurt: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.H),
      heal: scene.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.J),
    };
  }

  public attack(): boolean {
    if (!canPerformAttack(this.inventory.hasSword, this.isAttacking, this.attackCooldown)) {
      return false;
    }

    this.isAttacking = true;
    this.attackTimer = 180;
    this.attackCooldown = 240;
    this.setVelocity(0, 0);

    AudioSystem.getInstance().playSwordSlash();

    const offsets: Record<Direction, { x: number; y: number }> = {
      down: { x: 0, y: 14 },
      up: { x: 0, y: -14 },
      left: { x: -14, y: 2 },
      right: { x: 14, y: 2 },
    };
    const offset = offsets[this.facing];

    if (this.slashSprite) {
      this.slashSprite.destroy();
    }
    this.slashSprite = this.scene.add.sprite(
      this.x + offset.x,
      this.y + offset.y,
      `slash_${this.facing}`
    );
    this.slashSprite.setDepth(this.depth + 1);

    return true;
  }

  public getSwordHitbox(): HitboxRect | null {
    if (!this.isAttacking) {
      return null;
    }
    return getSwordHitbox({ x: this.x, y: this.y }, this.facing);
  }

  public takeDamage(amount: number): DamageResult {
    const result = this.health.takeDamage(amount);

    if (result.damaged) {
      AudioSystem.getInstance().playPlayerHurt();
      this.camerasFlash();
    }

    if (result.isDead) {
      this.setVelocity(0, 0);
      this.setTint(0xff0000);
      if (this.slashSprite) {
        this.slashSprite.destroy();
        this.slashSprite = undefined;
      }
      this.isAttacking = false;
    }

    return result;
  }

  private camerasFlash(): void {
    // Subtle red hit flash on camera
    this.scene.cameras.main.flash(150, 180, 0, 0, true);
  }

  destroy(fromScene?: boolean): void {
    if (this.slashSprite) {
      this.slashSprite.destroy();
      this.slashSprite = undefined;
    }
    super.destroy(fromScene);
  }

  update(_time?: number, delta: number = 16.6): void {
    // Update invulnerability cooldown
    this.health.update(delta);

    // Update attack cooldown and swing duration
    if (this.attackCooldown > 0) {
      this.attackCooldown -= delta;
    }

    if (this.isAttacking) {
      this.attackTimer -= delta;
      if (this.attackTimer <= 0) {
        this.isAttacking = false;
        if (this.slashSprite) {
          this.slashSprite.destroy();
          this.slashSprite = undefined;
        }
      }
    }

    // Handle debug manual damage/healing testing
    if (this.debugKeys) {
      if (Phaser.Input.Keyboard.JustDown(this.debugKeys.hurt)) {
        this.takeDamage(1);
      }
      if (Phaser.Input.Keyboard.JustDown(this.debugKeys.heal)) {
        this.health.heal(1);
      }
    }

    // Flash sprite alpha while invulnerable
    if (this.health.isInvulnerable() && !this.health.isDead()) {
      const flash = Math.floor(this.health.getInvulnerabilityTimer() / 80) % 2 === 0;
      this.setAlpha(flash ? 0.3 : 1.0);
    } else if (this.alpha !== 1.0) {
      this.setAlpha(1.0);
    }

    if (this.health.isDead()) {
      this.setVelocity(0, 0);
      return;
    }

    // Process attack input if can move and not currently swinging
    if (this.canMove && this.attackKeys) {
      if (
        Phaser.Input.Keyboard.JustDown(this.attackKeys.space) ||
        Phaser.Input.Keyboard.JustDown(this.attackKeys.z)
      ) {
        if (this.attack()) {
          return;
        }
      }
    }

    // Movement locked during attack or interaction
    if (!this.canMove || this.isAttacking) {
      this.setVelocity(0, 0);
      return;
    }

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
