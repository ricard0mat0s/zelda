/**
 * Procedural 8-bit Sound Synthesizer (ZzFX Core)
 * Pure, zero-dependency audio waveform generator for retro sound effects.
 */

export type SoundParams = number[];

export type SoundEffectName =
  | 'gameStart'
  | 'swordSlash'
  | 'arrowShoot'
  | 'hit'
  | 'enemyDefeat'
  | 'secretChime'
  | 'chestFanfare'
  | 'playerHurt'
  | 'bossRoar';

/**
 * Curated 8-bit retro sound presets for Zelda Adventure
 * [volume, randomness, frequency, attack, sustain, release, shape, shapeCurve, slide, deltaSlide, pitchJump, pitchJumpTime, repeatTime, noise, modulation, bitCrush, delay, sustainVolume, decay, tremolo]
 */
export const SOUND_PRESETS: Record<SoundEffectName, SoundParams> = {
  // Game Start / Title Select: Crisp ascending ding
  gameStart: [1.2, 0.01, 587, 0.01, 0.15, 0.25, 0, 1.5, 0, 0, 220, 0.08, 0, 0, 0, 0, 0.1, 0.8, 0.05, 0],

  // Sword Slash: Fast airy whoosh with metallic edge
  swordSlash: [1.5, 0.1, 440, 0.01, 0.04, 0.08, 3, 1.2, -8, -12, 0, 0, 0, 1.8, 0, 0, 0, 0.5, 0.02, 0],

  // Arrow Shoot: High-speed mechanical release twang
  arrowShoot: [1.2, 0.05, 880, 0.01, 0.03, 0.12, 2, 1.8, -15, -5, 0, 0, 0, 0.2, 0, 0, 0, 0.4, 0.02, 0],

  // Enemy / Shield Hit: Heavy blunt impact with retro bitcrush
  hit: [1.6, 0.05, 140, 0.01, 0.03, 0.1, 3, 2, -2, -6, 0, 0, 0, 0.8, 0, 3, 0, 0.7, 0.02, 0],

  // Enemy Defeat: Crumbling explosion pop
  enemyDefeat: [1.5, 0.1, 180, 0.01, 0.08, 0.22, 4, 1.5, -4, -4, 0, 0, 0, 2.5, 0, 4, 0.05, 0.6, 0.05, 0],

  // Secret Chime: Zelda-style mystery/discovery chime
  secretChime: [1.3, 0, 659, 0.01, 0.2, 0.35, 0, 1.2, 0, 0, 330, 0.12, 0, 0, 0, 0, 0.15, 0.9, 0.05, 0],

  // Chest Fanfare: Triumphant treasure chime
  chestFanfare: [1.4, 0.01, 523, 0.02, 0.3, 0.4, 1, 1.4, 0, 0, 261, 0.14, 0, 0, 0, 0, 0.12, 0.9, 0.08, 0],

  // Player Hurt: Grunting crunch
  playerHurt: [1.8, 0.05, 110, 0.01, 0.06, 0.18, 3, 2.5, -3, -8, 0, 0, 0, 1.2, 0, 5, 0, 0.8, 0.03, 0],

  // Boss Roar: Low rumble growl
  bossRoar: [2.0, 0.2, 65, 0.05, 0.45, 0.4, 3, 2, -1, -2, -20, 0.15, 0, 1.5, 10, 6, 0.1, 0.9, 0.1, 0.2],
};

/**
 * Pure mathematical PCM audio generator based on ZzFX.
 * Generates an array of floating point audio samples in the range [-1.0, 1.0].
 */
export function generateAudioSamples(params: SoundParams, sampleRate = 44100): Float32Array {
  // Destructure parameters with defaults
  const volume = params[0] ?? 1;
  const randomness = params[1] ?? 0.05;
  const frequency = params[2] ?? 220;
  const attack = params[3] ?? 0;
  const sustain = params[4] ?? 0;
  const release = params[5] ?? 0.1;
  const shape = params[6] ?? 0;
  const shapeCurve = params[7] ?? 1;
  let slide = params[8] ?? 0;
  const deltaSlide = params[9] ?? 0;
  const pitchJump = params[10] ?? 0;
  const pitchJumpTime = params[11] ?? 0;
  const repeatTime = params[12] ?? 0;
  const noise = params[13] ?? 0;
  const modulation = params[14] ?? 0;
  const bitCrush = params[15] ?? 0;
  const delay = params[16] ?? 0;
  const sustainVolume = params[17] ?? 1;
  const decay = params[18] ?? 0;
  const tremolo = params[19] ?? 0;

  // Calculate timing & lengths
  const PI2 = Math.PI * 2;
  const startSlide = slide * 500 * PI2 / sampleRate / sampleRate;
  const startDeltaSlide = deltaSlide * 2000 * PI2 / (sampleRate ** 3);
  let currentSlide = startSlide;

  let freq = frequency * (1 + randomness * (Math.random() * 2 - 1)) * PI2 / sampleRate;
  let modFreq = modulation * PI2 / sampleRate;
  let modPhase = 0;
  let phase = 0;
  let sampleCount = 0;
  let repeatCount = 0;

  // Envelope segment lengths
  const attackSamples = Math.max(1, Math.floor(attack * sampleRate));
  const decaySamples = Math.floor(decay * sampleRate);
  const sustainSamples = Math.floor(sustain * sampleRate);
  const releaseSamples = Math.max(1, Math.floor(release * sampleRate));
  const delaySamples = Math.floor(delay * sampleRate);
  const totalSamples = attackSamples + decaySamples + sustainSamples + releaseSamples + delaySamples;

  const samples = new Float32Array(totalSamples);
  const delayBuffer = delaySamples > 0 ? new Float32Array(delaySamples) : null;
  let delayIndex = 0;

  let bitCrushSample = 0;

  for (let i = 0; i < totalSamples; i++) {
    // Envelope computation
    let envelope = 0;
    if (i < attackSamples) {
      envelope = i / attackSamples;
    } else if (i < attackSamples + decaySamples) {
      const decayProgress = (i - attackSamples) / decaySamples;
      envelope = 1 - (1 - sustainVolume) * decayProgress;
    } else if (i < attackSamples + decaySamples + sustainSamples) {
      envelope = sustainVolume;
    } else if (i < totalSamples - delaySamples) {
      const releaseProgress = (i - attackSamples - decaySamples - sustainSamples) / releaseSamples;
      envelope = sustainVolume * (1 - releaseProgress);
    }
    envelope = Math.max(0, Math.min(1, envelope));
    if (shapeCurve !== 1) {
      envelope = envelope ** shapeCurve;
    }

    // Tremolo (volume modulation)
    if (tremolo > 0) {
      envelope *= 1 - tremolo * (0.5 + 0.5 * Math.sin(i * 12 * PI2 / sampleRate));
    }

    // Frequency slide & modulation
    currentSlide += startDeltaSlide;
    freq += currentSlide;
    if (modulation > 0) {
      modPhase += modFreq;
      phase += freq + Math.sin(modPhase) * (freq * 0.5);
    } else {
      phase += freq;
    }

    // Pitch jump
    if (pitchJumpTime > 0 && sampleCount > pitchJumpTime * sampleRate) {
      freq += pitchJump * PI2 / sampleRate;
      sampleCount = 0;
    }
    sampleCount++;

    // Repeat sound
    if (repeatTime > 0 && ++repeatCount > repeatTime * sampleRate) {
      freq = frequency * (1 + randomness * (Math.random() * 2 - 1)) * PI2 / sampleRate;
      currentSlide = startSlide;
      repeatCount = 0;
    }

    // Oscillator waveform synthesis
    let sample = 0;
    const oscPhase = phase % PI2;
    switch (shape) {
      case 0: // Sine
        sample = Math.sin(oscPhase);
        break;
      case 1: // Triangle
        sample = Math.abs((oscPhase / Math.PI) - 1) * 2 - 1;
        break;
      case 2: // Sawtooth
        sample = 1 - (oscPhase / Math.PI);
        break;
      case 3: // Square
        sample = oscPhase < Math.PI ? 1 : -1;
        break;
      case 4: // Noise
        sample = Math.random() * 2 - 1;
        break;
      default:
        sample = Math.sin(oscPhase);
    }

    // Noise blending
    if (noise > 0) {
      sample = (1 - Math.min(1, noise)) * sample + Math.min(1, noise) * (Math.random() * 2 - 1);
    }

    // Bitcrush effect (sample rate & bit depth reduction)
    if (bitCrush > 0) {
      if (i % (bitCrush + 1) === 0) {
        bitCrushSample = Math.round(sample * 4) / 4;
      }
      sample = bitCrushSample;
    }

    // Apply envelope and master volume
    sample *= envelope * volume;

    // Delay echo effect
    if (delayBuffer && delaySamples > 0) {
      const delayed = delayBuffer[delayIndex];
      delayBuffer[delayIndex] = sample;
      delayIndex = (delayIndex + 1) % delaySamples;
      sample += delayed * 0.4;
    }

    // Hard clip to [-1.0, 1.0] to guarantee audio bounds safety
    samples[i] = Math.max(-1.0, Math.min(1.0, sample));
  }

  return samples;
}
