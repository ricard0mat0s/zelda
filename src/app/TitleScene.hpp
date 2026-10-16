#pragma once

namespace App {

class TitleScene {
public:
    TitleScene();

    void update(float dt);
    void render();

    bool isFinished() const { return m_startRequested; }
    void reset();

private:
    float m_timer = 0.0f;
    bool m_promptVisible = true;
    bool m_startRequested = false;
    float m_fadeProgress = 0.0f;
};

} // namespace App
