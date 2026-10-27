#pragma once

#include "core/types.hpp"

namespace App {

class TitleScene {
public:
    TitleScene();

    void update(float dt);
    void update(float dt, const Core::GameInput& input);
    void render();

    bool isFinished() const { return m_startRequested; }
    bool wantsContinue() const { return m_continueRequested; }
    bool hasSave() const { return m_hasSave; }
    bool isPromptVisible() const { return m_promptVisible; }
    void reset();

private:
    float m_timer = 0.0f;
    bool m_promptVisible = true;
    bool m_startRequested = false;
    bool m_continueRequested = false;
    bool m_hasSave = false;
    float m_fadeProgress = 0.0f;
};

} // namespace App
