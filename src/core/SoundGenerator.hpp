#pragma once

#include <vector>
#include <string>

namespace Core {

enum class SoundPreset {
    GameStart,
    SwordSlash,
    ArrowShoot,
    Hit,
    EnemyDefeat,
    SecretChime,
    ChestFanfare,
    PlayerHurt,
    BossRoar
};

struct SoundParams {
    float volume = 1.0f;
    float randomness = 0.05f;
    float frequency = 220.0f;
    float attack = 0.0f;
    float sustain = 0.0f;
    float release = 0.1f;
    int shape = 0; // 0: Sine, 1: Triangle, 2: Sawtooth, 3: Square, 4: Noise
    float shapeCurve = 1.0f;
    float slide = 0.0f;
    float deltaSlide = 0.0f;
    float pitchJump = 0.0f;
    float pitchJumpTime = 0.0f;
    float repeatTime = 0.0f;
    float noise = 0.0f;
    float modulation = 0.0f;
    float bitCrush = 0.0f;
    float delay = 0.0f;
    float sustainVolume = 1.0f;
    float decay = 0.0f;
    float tremolo = 0.0f;
};

SoundParams getPresetParams(SoundPreset preset);

// Synthesizes float PCM audio samples normalized in [-1.0, 1.0]
std::vector<float> generateAudioSamples(const SoundParams& params, int sampleRate = 44100);

} // namespace Core
