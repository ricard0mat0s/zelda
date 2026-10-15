/**
 * Pure domain typewriter model for progressive text disclosure in dialogue boxes.
 */
export class Typewriter {
  private fullText: string;
  private charDelayMs: number;
  private elapsedMs: number = 0;
  private charsRevealed: number = 0;
  private completed: boolean = false;

  constructor(fullText: string = '', charDelayMs: number = 30) {
    this.fullText = fullText;
    this.charDelayMs = Math.max(1, charDelayMs);
  }

  public reset(fullText: string): void {
    this.fullText = fullText;
    this.elapsedMs = 0;
    this.charsRevealed = 0;
    this.completed = false;
  }

  public update(deltaMs: number): boolean {
    if (this.completed) {
      return false;
    }

    this.elapsedMs += deltaMs;
    const newCount = Math.min(
      this.fullText.length,
      Math.floor(this.elapsedMs / this.charDelayMs)
    );

    const changed = newCount !== this.charsRevealed;
    this.charsRevealed = newCount;

    if (this.charsRevealed >= this.fullText.length) {
      this.completed = true;
    }

    return changed;
  }

  public skip(): void {
    this.charsRevealed = this.fullText.length;
    this.completed = true;
  }

  public getDisplayedText(): string {
    return this.fullText.slice(0, this.charsRevealed);
  }

  public isComplete(): boolean {
    return this.completed;
  }

  public getFullText(): string {
    return this.fullText;
  }
}
