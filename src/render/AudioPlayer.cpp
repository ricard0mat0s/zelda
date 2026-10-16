#include "render/AudioPlayer.hpp"
#include <iostream>
#include <vector>

namespace Render {

AudioPlayer& AudioPlayer::instance() {
    static AudioPlayer s_instance;
    return s_instance;
}

AudioPlayer::~AudioPlayer() {
    cleanup();
}

Sound AudioPlayer::loadSoundFromPreset(Core::SoundPreset preset) {
    Core::SoundParams params = Core::getPresetParams(preset);
    constexpr int sampleRate = 44100;
    std::vector<float> floatSamples = Core::generateAudioSamples(params, sampleRate);

    std::vector<short> pcm16(floatSamples.size());
    for (size_t i = 0; i < floatSamples.size(); ++i) {
        float s = floatSamples[i];
        if (s > 1.0f) s = 1.0f;
        if (s < -1.0f) s = -1.0f;
        pcm16[i] = static_cast<short>(s * 32767.0f);
    }

    Wave wave;
    wave.frameCount = static_cast<unsigned int>(pcm16.size());
    wave.sampleRate = static_cast<unsigned int>(sampleRate);
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = pcm16.data();

    Sound sound = LoadSoundFromWave(wave);
    return sound;
}

void AudioPlayer::init() {
    if (m_initialized) return;

    const Core::SoundPreset allPresets[] = {
        Core::SoundPreset::GameStart,
        Core::SoundPreset::SwordSlash,
        Core::SoundPreset::ArrowShoot,
        Core::SoundPreset::Hit,
        Core::SoundPreset::EnemyDefeat,
        Core::SoundPreset::SecretChime,
        Core::SoundPreset::ChestFanfare,
        Core::SoundPreset::PlayerHurt,
        Core::SoundPreset::BossRoar,
    };

    for (auto preset : allPresets) {
        Sound s = loadSoundFromPreset(preset);
        m_sounds[static_cast<int>(preset)] = s;
    }

    m_initialized = true;
    std::cout << "[AudioPlayer] Synthesized and cached " << m_sounds.size() << " 8-bit sound effects." << std::endl;
}

void AudioPlayer::cleanup() {
    if (!m_initialized) return;

    for (auto& [_, sound] : m_sounds) {
        UnloadSound(sound);
    }
    m_sounds.clear();
    m_initialized = false;
}

void AudioPlayer::play(Core::SoundPreset preset) {
    auto it = m_sounds.find(static_cast<int>(preset));
    if (it != m_sounds.end()) {
        PlaySound(it->second);
    }
}

} // namespace Render
