import Phaser from 'phaser';
import { generateGameTextures } from '../utils/AssetGenerator';
import { Player } from '../entities/Player';
import { createRetroTextStyle, createRetroUITextStyle } from '../utils/text';
import { MAP_CONFIG } from '../constants/map';

import { DialogueBox } from '../ui/DialogueBox';
import { AudioSystem } from '../systems/AudioSystem';

export class OverworldScene extends Phaser.Scene {
  public player!: Player;
  public shrinePedestal!: Phaser.Physics.Arcade.Sprite;
  public dialogueBox!: DialogueBox;
  private obstacles!: Phaser.Physics.Arcade.StaticGroup;
  private interactPrompt!: Phaser.GameObjects.Text;
  private interactKeys?: {
    e: Phaser.Input.Keyboard.Key;
    enter: Phaser.Input.Keyboard.Key;
    space: Phaser.Input.Keyboard.Key;
  };

  public static readonly MAP_COLS = MAP_CONFIG.COLS;
  public static readonly MAP_ROWS = MAP_CONFIG.ROWS;
  public static readonly TILE_SIZE = MAP_CONFIG.TILE_SIZE;
  public static readonly WORLD_WIDTH = MAP_CONFIG.WORLD_WIDTH;
  public static readonly WORLD_HEIGHT = MAP_CONFIG.WORLD_HEIGHT;

  constructor() {
    super('OverworldScene');
  }

  create(): void {
    // Generate all procedural 16x16 pixel art textures
    generateGameTextures(this);

    // Setup world and physics bounds
    this.physics.world.setBounds(0, 0, OverworldScene.WORLD_WIDTH, OverworldScene.WORLD_HEIGHT);

    // Static physics group for solid collisions (trees, water, stone walls)
    this.obstacles = this.physics.add.staticGroup();

    this.buildOverworldMap();

    // Spawn player in the central-south forest clearing
    const spawnX = 200;
    const spawnY = 280;
    this.player = new Player(this, spawnX, spawnY);

    // Elder Shrine Pedestal holding the Hero's Sword
    this.shrinePedestal = this.physics.add.staticSprite(200, 180, 'shrine_pedestal_sword');
    this.physics.add.collider(this.player, this.shrinePedestal);

    // Floating interaction prompt above pedestal
    this.interactPrompt = this.add.text(200, 162, '[E] EXAMINE', createRetroUITextStyle({
      fontSize: '7px',
      color: '#ffd700',
      stroke: '#0b0c10',
      strokeThickness: 2,
    })).setOrigin(0.5).setVisible(false).setDepth(20);

    // Dialogue Box container
    this.dialogueBox = new DialogueBox(this);

    // Keyboard controls for interaction
    if (this.input.keyboard) {
      this.interactKeys = {
        e: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.E),
        enter: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.ENTER),
        space: this.input.keyboard.addKey(Phaser.Input.Keyboard.KeyCodes.SPACE),
      };
    }

    // Launch parallel HUD overlay with player's health model
    this.scene.launch('UIScene', { health: this.player.health });

    // Register collisions
    this.physics.add.collider(this.player, this.obstacles);

    // Camera setup: bounds clamped to 40x30 map and smooth tracking on the Hero
    const camera = this.cameras.main;
    camera.setBounds(0, 0, OverworldScene.WORLD_WIDTH, OverworldScene.WORLD_HEIGHT);
    camera.startFollow(this.player, true, 0.1, 0.1);
    camera.fadeIn(400, 0, 0, 0);

    // Introductory location banner that floats and fades out
    this.showLocationBanner();
  }

  private buildOverworldMap(): void {
    const cols = OverworldScene.MAP_COLS;
    const rows = OverworldScene.MAP_ROWS;
    const tileSize = OverworldScene.TILE_SIZE;

    // 1. Base Layer: Fill entire map with grass
    for (let y = 0; y < rows; y++) {
      for (let x = 0; x < cols; x++) {
        this.add.image(x * tileSize + 8, y * tileSize + 8, 'grass');
      }
    }

    // 2. Pathways: Draw trails connecting areas
    // Central path going north
    for (let y = 8; y <= 24; y++) {
      this.add.image(12 * tileSize + 8, y * tileSize + 8, 'path');
      this.add.image(13 * tileSize + 8, y * tileSize + 8, 'path');
    }
    // Horizontal cross path
    for (let x = 6; x <= 32; x++) {
      this.add.image(x * tileSize + 8, 17 * tileSize + 8, 'path');
    }
    // Trail leading to northern ruins/dungeon
    for (let y = 3; y <= 8; y++) {
      this.add.image(20 * tileSize + 8, y * tileSize + 8, 'path');
    }

    // 3. Obstacles (Trees, Water, Walls)
    for (let y = 0; y < rows; y++) {
      for (let x = 0; x < cols; x++) {
        const posX = x * tileSize + 8;
        const posY = y * tileSize + 8;

        // Perimeter barrier trees (2-tile thick outer forest border)
        const isPerimeter = x <= 1 || x >= cols - 2 || y <= 1 || y >= rows - 2;

        if (isPerimeter) {
          this.obstacles.create(posX, posY, 'tree');
          continue;
        }

        // Northeastern Water Pond (x: 26..33, y: 4..9)
        const isPond = x >= 26 && x <= 33 && y >= 4 && y <= 9;
        if (isPond) {
          this.obstacles.create(posX, posY, 'water');
          continue;
        }

        // Northern Stone Ruins (x: 17..23, y: 2..5)
        const isRuinsWall = (y === 2 && x >= 17 && x <= 23) ||
                            (x === 17 && y >= 3 && y <= 4) ||
                            (x === 23 && y >= 3 && y <= 4);
        if (isRuinsWall) {
          this.obstacles.create(posX, posY, 'stone_wall');
          continue;
        }

        // Tree clusters creating natural forest clearings
        const isForestCluster1 = x >= 4 && x <= 8 && y >= 4 && y <= 7;
        const isForestCluster2 = x >= 5 && x <= 9 && y >= 21 && y <= 25;
        const isForestCluster3 = x >= 24 && x <= 28 && y >= 20 && y <= 24;

        if (isForestCluster1 || isForestCluster2 || isForestCluster3) {
          this.obstacles.create(posX, posY, 'tree');
        }
      }
    }

    // Refresh static body states for arcade physics
    this.obstacles.refresh();
  }

  private showLocationBanner(): void {
    // Floating location HUD text clamped to screen
    const banner = this.add.text(160, 24, 'HYLIA GLADE', createRetroTextStyle({
      fontSize: '10px',
      color: '#ffd700',
      stroke: '#0b0c10',
      strokeThickness: 3,
    })).setOrigin(0.5).setScrollFactor(0);

    const subBanner = this.add.text(160, 36, 'THE OVERWORLD', createRetroUITextStyle({
      fontSize: '7px',
      color: '#d8f3dc',
      stroke: '#0b0c10',
      strokeThickness: 2,
    })).setOrigin(0.5).setScrollFactor(0);

    // Fade out banner after 3 seconds
    this.time.delayedCall(2500, () => {
      this.tweens.add({
        targets: [banner, subBanner],
        alpha: 0,
        duration: 800,
        onComplete: () => {
          banner.destroy();
          subBanner.destroy();
        },
      });
    });
  }

  public showNotification(message: string): void {
    const banner = this.add.text(160, 60, message, createRetroTextStyle({
      fontSize: '9px',
      color: '#00f5d4',
      stroke: '#0b0c10',
      strokeThickness: 3,
    })).setOrigin(0.5).setScrollFactor(0).setDepth(1001);

    this.tweens.add({
      targets: banner,
      y: 50,
      alpha: 0,
      delay: 1800,
      duration: 600,
      onComplete: () => {
        banner.destroy();
      },
    });
  }

  update(time: number, delta: number): void {
    if (this.dialogueBox) {
      this.dialogueBox.update(time, delta);
    }

    if (this.player && this.shrinePedestal) {
      const dist = Phaser.Math.Distance.Between(
        this.player.x,
        this.player.y,
        this.shrinePedestal.x,
        this.shrinePedestal.y
      );
      const inRange = dist < 26;

      if (inRange && !this.dialogueBox.isOpen()) {
        this.interactPrompt.setVisible(true);
      } else {
        this.interactPrompt.setVisible(false);
      }

      if (this.interactKeys) {
        const interactJustDown =
          Phaser.Input.Keyboard.JustDown(this.interactKeys.e) ||
          Phaser.Input.Keyboard.JustDown(this.interactKeys.enter);

        if (this.dialogueBox.isOpen()) {
          const advanceKey =
            interactJustDown || Phaser.Input.Keyboard.JustDown(this.interactKeys.space);
          if (advanceKey) {
            const closed = this.dialogueBox.advance();
            if (closed) {
              this.player.canMove = true;
            }
          }
        } else if (inRange && interactJustDown) {
          this.player.canMove = false;
          this.player.setVelocity(0, 0);
          this.interactPrompt.setVisible(false);

          if (!this.player.inventory.hasSword) {
            this.dialogueBox.show("IT'S DANGEROUS TO GO ALONE!\nTAKE THIS.", () => {
              this.player.inventory.obtainSword();
              this.shrinePedestal.setTexture('shrine_pedestal_empty');
              AudioSystem.getInstance().playSecretChime();
              this.showNotification("HERO'S SWORD ACQUIRED!");
            });
          } else {
            this.dialogueBox.show('THE ANCIENT PEDESTAL IS EMPTY.');
          }
        }
      }
    }

    if (this.player) {
      this.player.update(time, delta);
    }
  }
}
