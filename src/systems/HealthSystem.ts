import { HeartDisplayUnit } from '../types/game';

export interface DamageResult {
  damaged: boolean;
  isDead: boolean;
  remainingHP: number;
}

export type HealthChangeCallback = (stats: {
  currentHP: number;
  maxHP: number;
  units: HeartDisplayUnit[];
}) => void;

export type DeathCallback = () => void;

/**
 * HealthSystem is a pure, testable domain model managing heart health,
 * half-heart units, healing, invulnerability cooldowns, and death state.
 */
export class HealthSystem {
  public static readonly HP_PER_HEART = 2;
  public static readonly DEFAULT_INVULNERABILITY_MS = 1000;

  private maxHearts: number;
  private currentHP: number;
  private invulnerabilityTimer: number = 0;

  private healthChangeListeners: HealthChangeCallback[] = [];
  private deathListeners: DeathCallback[] = [];

  constructor(initialHearts: number = 3) {
    this.maxHearts = Math.max(1, initialHearts);
    this.currentHP = this.maxHearts * HealthSystem.HP_PER_HEART;
  }

  public getMaxHearts(): number {
    return this.maxHearts;
  }

  public getMaxHP(): number {
    return this.maxHearts * HealthSystem.HP_PER_HEART;
  }

  public getCurrentHP(): number {
    return this.currentHP;
  }

  public isDead(): boolean {
    return this.currentHP <= 0;
  }

  public isInvulnerable(): boolean {
    return this.invulnerabilityTimer > 0;
  }

  public getInvulnerabilityTimer(): number {
    return this.invulnerabilityTimer;
  }

  /**
   * Applies damage if not currently invulnerable and not already dead.
   * Activates invulnerability cooldown upon successful hit.
   */
  public takeDamage(amount: number): DamageResult {
    if (this.isDead() || this.isInvulnerable() || amount <= 0) {
      return {
        damaged: false,
        isDead: this.isDead(),
        remainingHP: this.currentHP,
      };
    }

    this.currentHP = Math.max(0, this.currentHP - amount);
    this.invulnerabilityTimer = HealthSystem.DEFAULT_INVULNERABILITY_MS;

    const dead = this.isDead();
    this.notifyHealthChanged();

    if (dead) {
      this.notifyDied();
    }

    return {
      damaged: true,
      isDead: dead,
      remainingHP: this.currentHP,
    };
  }

  /**
   * Heals the entity up to maximum HP. Cannot heal if dead.
   */
  public heal(amount: number): void {
    if (this.isDead() || amount <= 0) {
      return;
    }

    const previousHP = this.currentHP;
    this.currentHP = Math.min(this.getMaxHP(), this.currentHP + amount);

    if (this.currentHP !== previousHP) {
      this.notifyHealthChanged();
    }
  }

  /**
   * Adds a permanent Heart Container (+1 heart = +2 HP) and fully restores health.
   */
  public addHeartContainer(): void {
    this.maxHearts++;
    this.currentHP = this.getMaxHP();
    this.notifyHealthChanged();
  }

  /**
   * Advances game time, reducing invulnerability cooldown.
   */
  public update(deltaMs: number): void {
    if (this.invulnerabilityTimer > 0) {
      this.invulnerabilityTimer = Math.max(0, this.invulnerabilityTimer - deltaMs);
    }
  }

  /**
   * Computes display states for every heart container ('full' | 'half' | 'empty').
   */
  public getHeartUnits(): HeartDisplayUnit[] {
    const units: HeartDisplayUnit[] = [];
    let hpToDistribute = this.currentHP;

    for (let i = 0; i < this.maxHearts; i++) {
      if (hpToDistribute >= 2) {
        units.push({ type: 'full' });
        hpToDistribute -= 2;
      } else if (hpToDistribute === 1) {
        units.push({ type: 'half' });
        hpToDistribute -= 1;
      } else {
        units.push({ type: 'empty' });
      }
    }

    return units;
  }

  public onHealthChanged(callback: HealthChangeCallback): void {
    this.healthChangeListeners.push(callback);
  }

  public onDied(callback: DeathCallback): void {
    this.deathListeners.push(callback);
  }

  private notifyHealthChanged(): void {
    const stats = {
      currentHP: this.currentHP,
      maxHP: this.getMaxHP(),
      units: this.getHeartUnits(),
    };
    for (const listener of this.healthChangeListeners) {
      listener(stats);
    }
  }

  private notifyDied(): void {
    for (const listener of this.deathListeners) {
      listener();
    }
  }
}
