import { describe, it, expect, beforeEach } from 'vitest';
import { AudioSystem } from './AudioSystem';

describe('AudioSystem', () => {
  let audio: AudioSystem;

  beforeEach(() => {
    audio = AudioSystem.getInstance();
    audio.setMuted(false);
    audio.setMasterVolume(1.0);
  });

  it('manages singleton instance cleanly', () => {
    const instanceA = AudioSystem.getInstance();
    const instanceB = AudioSystem.getInstance();
    expect(instanceA).toBe(instanceB);
  });

  it('handles mute state correctly', () => {
    expect(audio.isMuted()).toBe(false);
    audio.setMuted(true);
    expect(audio.isMuted()).toBe(true);
    audio.setMuted(false);
    expect(audio.isMuted()).toBe(false);
  });

  it('clamps master volume between 0.0 and 1.0', () => {
    audio.setMasterVolume(0.5);
    expect(audio.getMasterVolume()).toBe(0.5);

    audio.setMasterVolume(2.5);
    expect(audio.getMasterVolume()).toBe(1.0);

    audio.setMasterVolume(-0.8);
    expect(audio.getMasterVolume()).toBe(0.0);
  });

  it('does not throw when play methods are called without an active Web Audio device (headless resilience)', () => {
    expect(() => {
      audio.playGameStart();
      audio.playSwordSlash();
      audio.playArrowShoot();
      audio.playHit();
      audio.playEnemyDefeat();
      audio.playSecretChime();
      audio.playChestFanfare();
      audio.playPlayerHurt();
      audio.playBossRoar();
    }).not.toThrow();
  });
});
