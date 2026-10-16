#pragma once

#include "raylib.h"
#include "core/GameConfig.hpp"

namespace Render {

class VirtualCanvas {
public:
    VirtualCanvas();
    ~VirtualCanvas();

    void init();
    void close();

    void beginVirtual();
    void endVirtual();

    void drawScaled();

    int getVirtualWidth() const { return Core::GameConfig::VIRTUAL_WIDTH; }
    int getVirtualHeight() const { return Core::GameConfig::VIRTUAL_HEIGHT; }

private:
    RenderTexture2D m_target{};
    bool m_initialized = false;
};

} // namespace Render
