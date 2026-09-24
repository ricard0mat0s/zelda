import { generateAudioSamples, SOUND_PRESETS, SoundEffectName } from './audio/zzfx';

/**
 * AudioSystem manages Web Audio context, buffer caching, volume, and playback.
 * Synthesizes 8-bit sounds on demand using ZzFX and caches them in memory.
 */
export class AudioSystem {
  private static instance: AudioSystem;

  private audioCtx: AudioContext | null = null;
  private bufferCache = new Map<SoundEffectName, AudioBuffer>();
  private masterVolume: number = 1.0;
  private muted: boolean = false;

  private constructor() {}

  public static getInstance(): AudioSystem {
    if (!AudioSystem.instance) {
      AudioSystem.instance = new AudioSystem();
    }
    return AudioSystem.instance;
  }

  /**
   * Initializes or resumes the Web Audio context after a user gesture.
   */
  public initContext(): AudioContext | null {
    if (typeof window === 'undefined') {
      return null;
    }

    if (!this.audioCtx) {
      const AudioCtxClass = window.AudioContext || (window as unknown as { webkitAudioContext: typeof AudioContext }).webkitAudioContext;
      if (AudioCtxClass) {
        this.audioCtx = new AudioCtxClass();
      }
    }

    if (this.audioCtx && this.audioCtx.state === 'suspended') {
      this.audioCtx.resume().catch(() => {
        // Ignored if browser blocks resume
      });
    }

    return this.audioCtx;
  }

  public setMuted(muted: boolean): void {
    this.muted = muted;
  }

  public isMuted(): boolean {
    return this.muted;
  }

  public setMasterVolume(vol: number): void {
    this.masterVolume = Math.max(0, Math.min(1, vol));
  }

  public getMasterVolume(): number {
    return this.masterVolume;
  }

  /**
   * Plays a sound effect by name. Caches AudioBuffers for fast zero-latency replaying.
   */
  public play(name: SoundEffectName, volumeScale: number = 1.0): void {
    if (this.muted || this.masterVolume <= 0) {
      return;
    }

    const ctx = this.initContext();
    if (!ctx) {
      return;
    }

    try {
      let audioBuffer = this.bufferCache.get(name);
      if (!audioBuffer) {
        const params = SOUND_PRESETS[name];
        if (!params) return;

        const samples = generateAudioSamples(params, ctx.sampleRate);
        audioBuffer = ctx.createBuffer(1, samples.length, ctx.sampleRate);
        audioBuffer.getChannelData(0).set(samples);
        this.bufferCache.set(name, audioBuffer);
      }

      const source = ctx.createBufferSource();
      source.buffer = audioBuffer;

      const gainNode = ctx.createGain();
      gainNode.gain.value = this.masterVolume * volumeScale;

      source.connect(gainNode);
      gainNode.connect(ctx.destination);

      source.start();
    } catch {
      // Graceful fallback if audio context has issues
    }
  }

  // Convenience methods for Zelda sound effects
  public playGameStart(): void {
    this.play('gameStart', 1.0);
  }

  public playSwordSlash(): void {
    this.play('swordSlash', 0.9);
  }

  public playArrowShoot(): void {
    this.play('arrowShoot', 0.85);
  }

  public playHit(): void {
    this.play('hit', 1.0);
  }

  public playEnemyDefeat(): void {
    this.play('enemyDefeat', 1.0);
  }

  public playSecretChime(): void {
    this.play('secretChime', 1.0);
  }

  public playChestFanfare(): void {
    this.play('chestFanfare', 1.0);
  }

  public playPlayerHurt(): void {
    this.play('playerHurt', 1.0);
  }

  public playBossRoar(): void {
    this.play('bossRoar', 1.0);
  }
}
