#include "core/SoundGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace Core {

SoundParams getPresetParams(SoundPreset preset) {
    switch (preset) {
        case SoundPreset::GameStart:
            return {1.2f, 0.01f, 587.0f, 0.01f, 0.15f, 0.25f, 0, 1.5f, 0.0f, 0.0f, 220.0f, 0.08f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.8f, 0.05f, 0.0f};
        case SoundPreset::SwordSlash:
            return {1.5f, 0.1f, 440.0f, 0.01f, 0.04f, 0.08f, 3, 1.2f, -8.0f, -12.0f, 0.0f, 0.0f, 0.0f, 1.8f, 0.0f, 0.0f, 0.0f, 0.5f, 0.02f, 0.0f};
        case SoundPreset::ArrowShoot:
            return {1.2f, 0.05f, 880.0f, 0.01f, 0.03f, 0.12f, 2, 1.8f, -15.0f, -5.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.4f, 0.02f, 0.0f};
        case SoundPreset::Hit:
            return {1.6f, 0.05f, 140.0f, 0.01f, 0.03f, 0.1f, 3, 2.0f, -2.0f, -6.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 3.0f, 0.0f, 0.7f, 0.02f, 0.0f};
        case SoundPreset::EnemyDefeat:
            return {1.5f, 0.1f, 180.0f, 0.01f, 0.08f, 0.22f, 4, 1.5f, -4.0f, -4.0f, 0.0f, 0.0f, 0.0f, 2.5f, 0.0f, 4.0f, 0.05f, 0.6f, 0.05f, 0.0f};
        case SoundPreset::SecretChime:
            return {1.3f, 0.0f, 659.0f, 0.01f, 0.2f, 0.35f, 0, 1.2f, 0.0f, 0.0f, 330.0f, 0.12f, 0.0f, 0.0f, 0.0f, 0.0f, 0.15f, 0.9f, 0.05f, 0.0f};
        case SoundPreset::ChestFanfare:
            return {1.4f, 0.01f, 523.0f, 0.02f, 0.3f, 0.4f, 1, 1.4f, 0.0f, 0.0f, 261.0f, 0.14f, 0.0f, 0.0f, 0.0f, 0.0f, 0.12f, 0.9f, 0.08f, 0.0f};
        case SoundPreset::PlayerHurt:
            return {1.8f, 0.05f, 110.0f, 0.01f, 0.06f, 0.18f, 3, 2.5f, -3.0f, -8.0f, 0.0f, 0.0f, 0.0f, 1.2f, 0.0f, 5.0f, 0.0f, 0.8f, 0.03f, 0.0f};
        case SoundPreset::BossRoar:
            return {2.0f, 0.2f, 65.0f, 0.05f, 0.45f, 0.4f, 3, 2.0f, -1.0f, -2.0f, -20.0f, 0.15f, 0.0f, 1.5f, 10.0f, 6.0f, 0.1f, 0.9f, 0.1f, 0.2f};
    }
    return {};
}

std::vector<float> generateAudioSamples(const SoundParams& params, int sampleRate) {
    constexpr float PI2 = 6.28318530717958647692f;
    std::mt19937 rng(42); // Deterministic pseudo-randomness for audio consistency
    std::uniform_real_distribution<float> distMinusOneToOne(-1.0f, 1.0f);

    const float startSlide = params.slide * 500.0f * PI2 / (sampleRate * sampleRate);
    const float startDeltaSlide = params.deltaSlide * 2000.0f * PI2 / (static_cast<float>(sampleRate) * sampleRate * sampleRate);
    float currentSlide = startSlide;

    float freq = params.frequency * (1.0f + params.randomness * distMinusOneToOne(rng)) * PI2 / sampleRate;
    float modFreq = params.modulation * PI2 / sampleRate;
    float modPhase = 0.0f;
    float phase = 0.0f;
    int sampleCount = 0;
    int repeatCount = 0;

    const int attackSamples = std::max(1, static_cast<int>(params.attack * sampleRate));
    const int decaySamples = static_cast<int>(params.decay * sampleRate);
    const int sustainSamples = static_cast<int>(params.sustain * sampleRate);
    const int releaseSamples = std::max(1, static_cast<int>(params.release * sampleRate));
    const int delaySamples = static_cast<int>(params.delay * sampleRate);
    const int totalSamples = attackSamples + decaySamples + sustainSamples + releaseSamples + delaySamples;

    std::vector<float> samples(totalSamples, 0.0f);
    std::vector<float> delayBuffer(delaySamples > 0 ? delaySamples : 1, 0.0f);
    int delayIndex = 0;
    float bitCrushSample = 0.0f;

    for (int i = 0; i < totalSamples; ++i) {
        // Envelope calculation
        float envelope = 0.0f;
        if (i < attackSamples) {
            envelope = static_cast<float>(i) / attackSamples;
        } else if (i < attackSamples + decaySamples) {
            float decayProgress = static_cast<float>(i - attackSamples) / decaySamples;
            envelope = 1.0f - (1.0f - params.sustainVolume) * decayProgress;
        } else if (i < attackSamples + decaySamples + sustainSamples) {
            envelope = params.sustainVolume;
        } else if (i < totalSamples - delaySamples) {
            float releaseProgress = static_cast<float>(i - attackSamples - decaySamples - sustainSamples) / releaseSamples;
            envelope = params.sustainVolume * (1.0f - releaseProgress);
        }
        envelope = std::clamp(envelope, 0.0f, 1.0f);
        if (params.shapeCurve != 1.0f && envelope > 0.0f) {
            envelope = std::pow(envelope, params.shapeCurve);
        }

        // Tremolo
        if (params.tremolo > 0.0f) {
            envelope *= 1.0f - params.tremolo * (0.5f + 0.5f * std::sin(i * 12.0f * PI2 / sampleRate));
        }

        // Frequency slide & modulation
        currentSlide += startDeltaSlide;
        freq += currentSlide;
        if (params.modulation > 0.0f) {
            modPhase += modFreq;
            phase += freq + std::sin(modPhase) * (freq * 0.5f);
        } else {
            phase += freq;
        }

        // Pitch jump
        if (params.pitchJumpTime > 0.0f && sampleCount > params.pitchJumpTime * sampleRate) {
            freq += params.pitchJump * PI2 / sampleRate;
            sampleCount = 0;
        }
        sampleCount++;

        // Repeat time
        if (params.repeatTime > 0.0f && ++repeatCount > params.repeatTime * sampleRate) {
            freq = params.frequency * (1.0f + params.randomness * distMinusOneToOne(rng)) * PI2 / sampleRate;
            currentSlide = startSlide;
            repeatCount = 0;
        }

        // Oscillator shape
        float sample = 0.0f;
        float oscPhase = std::fmod(phase, PI2);
        if (oscPhase < 0.0f) oscPhase += PI2;

        switch (params.shape) {
            case 0: // Sine
                sample = std::sin(oscPhase);
                break;
            case 1: // Triangle
                sample = std::abs((oscPhase / 3.14159265f) - 1.0f) * 2.0f - 1.0f;
                break;
            case 2: // Sawtooth
                sample = 1.0f - (oscPhase / 3.14159265f);
                break;
            case 3: // Square
                sample = oscPhase < 3.14159265f ? 1.0f : -1.0f;
                break;
            case 4: // Noise
                sample = distMinusOneToOne(rng);
                break;
            default:
                sample = std::sin(oscPhase);
                break;
        }

        // Noise blending
        if (params.noise > 0.0f) {
            float nBlend = std::min(1.0f, params.noise);
            sample = (1.0f - nBlend) * sample + nBlend * distMinusOneToOne(rng);
        }

        // Bitcrush
        if (params.bitCrush > 0.0f) {
            int step = static_cast<int>(params.bitCrush) + 1;
            if (i % step == 0) {
                bitCrushSample = std::round(sample * 4.0f) / 4.0f;
            }
            sample = bitCrushSample;
        }

        // Volume & envelope
        sample *= envelope * params.volume;

        // Delay echo
        if (delaySamples > 0) {
            float delayed = delayBuffer[delayIndex];
            delayBuffer[delayIndex] = sample;
            delayIndex = (delayIndex + 1) % delaySamples;
            sample += delayed * 0.4f;
        }

        // Hard clip to [-1.0, 1.0]
        samples[i] = std::clamp(sample, -1.0f, 1.0f);
    }

    return samples;
}

} // namespace Core
