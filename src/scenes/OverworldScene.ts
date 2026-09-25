import Phaser from 'phaser';
import { generateGameTextures } from '../utils/AssetGenerator';
import { Player } from '../entities/Player';
import { createRetroTextStyle, createRetroUITextStyle } from '../utils/text';
import { MAP_CONFIG } from '../constants/map';

export class OverworldScene extends Phaser.Scene {
  public player!: Player;
  private obstacles!: Phaser.Physics.Arcade.StaticGroup;

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

  update(): void {
    if (this.player) {
      this.player.update();
    }
  }
}
