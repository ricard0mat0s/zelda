import Phaser from 'phaser';
import { Typewriter } from './typewriter';
import { createRetroUITextStyle } from '../utils/text';

export class DialogueBox extends Phaser.GameObjects.Container {
  private typewriter: Typewriter;
  private bgGraphics: Phaser.GameObjects.Graphics;
  private messageText: Phaser.GameObjects.Text;
  private continuePrompt: Phaser.GameObjects.Text;
  private onCompleteCallback?: () => void;
  private promptTimer: number = 0;

  public static readonly BOX_WIDTH = 280;
  public static readonly BOX_HEIGHT = 58;

  constructor(scene: Phaser.Scene, x: number = 20, y: number = 170) {
    super(scene, x, y);

    this.typewriter = new Typewriter('', 28);

    // Frame graphics: dark background with retro double border
    this.bgGraphics = scene.add.graphics();
    this.drawBox(DialogueBox.BOX_WIDTH, DialogueBox.BOX_HEIGHT);
    this.add(this.bgGraphics);

    // Dialogue text
    this.messageText = scene.add.text(12, 10, '', createRetroUITextStyle({
      fontSize: '8px',
      color: '#ffffff',
      stroke: '#0b0c10',
      strokeThickness: 2,
      wordWrap: { width: DialogueBox.BOX_WIDTH - 24 },
    }));
    this.add(this.messageText);

    // Continue prompt
    this.continuePrompt = scene.add.text(
      DialogueBox.BOX_WIDTH - 12,
      DialogueBox.BOX_HEIGHT - 12,
      '▼ PRESS E',
      createRetroUITextStyle({
        fontSize: '7px',
        color: '#ffd700',
        stroke: '#0b0c10',
        strokeThickness: 2,
      })
    ).setOrigin(1, 0.5);
    this.continuePrompt.setVisible(false);
    this.add(this.continuePrompt);

    // Pin container to screen
    this.setScrollFactor(0);
    this.setDepth(1000);
    this.setVisible(false);

    scene.add.existing(this);
  }

  private drawBox(w: number, h: number): void {
    this.bgGraphics.clear();

    // Background fill
    this.bgGraphics.fillStyle(0x0b0c10, 0.95);
    this.bgGraphics.fillRect(0, 0, w, h);

    // Outer border (gold)
    this.bgGraphics.lineStyle(2, 0xffd700, 1.0);
    this.bgGraphics.strokeRect(1, 1, w - 2, h - 2);

    // Inner border (slate)
    this.bgGraphics.lineStyle(1, 0x718096, 0.8);
    this.bgGraphics.strokeRect(4, 4, w - 8, h - 8);
  }

  public show(message: string, onComplete?: () => void): void {
    this.typewriter.reset(message);
    this.messageText.setText('');
    this.onCompleteCallback = onComplete;
    this.continuePrompt.setVisible(false);
    this.promptTimer = 0;
    this.setVisible(true);
  }

  public advance(): boolean {
    if (!this.visible) {
      return false;
    }

    if (!this.typewriter.isComplete()) {
      // Fast forward typewriter to end
      this.typewriter.skip();
      this.messageText.setText(this.typewriter.getFullText());
      this.continuePrompt.setVisible(true);
      return false;
    }

    // Dialogue finished, close it
    this.setVisible(false);
    const cb = this.onCompleteCallback;
    this.onCompleteCallback = undefined;
    cb?.();
    return true;
  }

  public isOpen(): boolean {
    return this.visible;
  }

  public update(_time: number, delta: number): void {
    if (!this.visible) {
      return;
    }

    if (!this.typewriter.isComplete()) {
      const changed = this.typewriter.update(delta);
      if (changed) {
        this.messageText.setText(this.typewriter.getDisplayedText());
      }
      if (this.typewriter.isComplete()) {
        this.continuePrompt.setVisible(true);
      }
    } else {
      // Blink continue indicator
      this.promptTimer += delta;
      this.continuePrompt.setVisible(Math.floor(this.promptTimer / 300) % 2 === 0);
    }
  }
}
