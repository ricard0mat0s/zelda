import { describe, it, expect } from 'vitest';
import { generateAudioSamples, SOUND_PRESETS, SoundEffectName } from './zzfx';

describe('ZzFX Procedural Sound Synthesizer', () => {
  it('generates a non-empty Float32Array within valid audio amplitude bounds [-1, 1]', () => {
    const samples = generateAudioSamples(SOUND_PRESETS.swordSlash);
    expect(samples).toBeInstanceOf(Float32Array);
    expect(samples.length).toBeGreaterThan(0);

    let hasNonZero = false;
    for (let i = 0; i < samples.length; i++) {
      const sample = samples[i];
      expect(Number.isFinite(sample)).toBe(true);
      expect(sample).toBeGreaterThanOrEqual(-1.0);
      expect(sample).toBeLessThanOrEqual(1.0);
      if (Math.abs(sample) > 0.001) {
        hasNonZero = true;
      }
    }
    expect(hasNonZero).toBe(true);
  });

  it('contains all required Zelda retro sound presets with valid sample generation', () => {
    const requiredSounds: SoundEffectName[] = [
      'gameStart',
      'swordSlash',
      'arrowShoot',
      'hit',
      'enemyDefeat',
      'secretChime',
      'chestFanfare',
      'playerHurt',
      'bossRoar',
    ];

    for (const name of requiredSounds) {
      expect(SOUND_PRESETS[name]).toBeDefined();
      const samples = generateAudioSamples(SOUND_PRESETS[name]);
      expect(samples.length).toBeGreaterThan(500); // At least ~11ms of audio at 44.1kHz
      expect(Number.isFinite(samples[0])).toBe(true);
    }
  });

  it('respects duration and sample rate configuration', () => {
    const sampleRate = 22050;
    // Short 0.1s sound
    const params = [1, 0, 440, 0.02, 0.05, 0.03]; // total ~0.1s
    const samples = generateAudioSamples(params, sampleRate);
    // Expected ~0.1s * 22050 = ~2205 samples (+/- small margin)
    expect(samples.length).toBeGreaterThan(1500);
    expect(samples.length).toBeLessThan(3500);
  });
});
