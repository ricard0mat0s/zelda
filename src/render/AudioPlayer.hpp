#pragma once

#include "raylib.h"
#include "core/SoundGenerator.hpp"
#include <unordered_map>

namespace Render {

class AudioPlayer {
public:
    static AudioPlayer& instance();

    void init();
    void cleanup();

    void play(Core::SoundPreset preset);

    void playGameStart()   { play(Core::SoundPreset::GameStart); }
    void playSwordSlash()  { play(Core::SoundPreset::SwordSlash); }
    void playArrowShoot()  { play(Core::SoundPreset::ArrowShoot); }
    void playHit()         { play(Core::SoundPreset::Hit); }
    void playEnemyDefeat() { play(Core::SoundPreset::EnemyDefeat); }
    void playSecretChime() { play(Core::SoundPreset::SecretChime); }
    void playChestFanfare(){ play(Core::SoundPreset::ChestFanfare); }
    void playPlayerHurt()  { play(Core::SoundPreset::PlayerHurt); }
    void playBossRoar()    { play(Core::SoundPreset::BossRoar); }

private:
    AudioPlayer() = default;
    ~AudioPlayer();

    Sound loadSoundFromPreset(Core::SoundPreset preset);

    std::unordered_map<int, Sound> m_sounds;
    bool m_initialized = false;
};

} // namespace Render
