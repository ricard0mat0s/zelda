#include "render/RenderUtils.hpp"
#include <algorithm>
#include <cmath>

namespace Render {

VirtualCanvas::VirtualCanvas() = default;

VirtualCanvas::~VirtualCanvas() {
    close();
}

void VirtualCanvas::init() {
    if (!m_initialized) {
        m_target = LoadRenderTexture(Core::GameConfig::VIRTUAL_WIDTH, Core::GameConfig::VIRTUAL_HEIGHT);
        SetTextureFilter(m_target.texture, TEXTURE_FILTER_POINT);
        m_initialized = true;
    }
}

void VirtualCanvas::close() {
    if (m_initialized) {
        UnloadRenderTexture(m_target);
        m_initialized = false;
    }
}

void VirtualCanvas::beginVirtual() {
    BeginTextureMode(m_target);
    ClearBackground(BLACK);
}

void VirtualCanvas::endVirtual() {
    EndTextureMode();
}

void VirtualCanvas::drawScaled() {
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());
    const float virtualW = static_cast<float>(Core::GameConfig::VIRTUAL_WIDTH);
    const float virtualH = static_cast<float>(Core::GameConfig::VIRTUAL_HEIGHT);

    // Compute integer scale if possible, otherwise aspect-ratio preserving float scale
    float scale = std::min(screenW / virtualW, screenH / virtualH);
    if (scale >= 1.0f) {
        scale = std::floor(scale);
    }

    const float renderW = virtualW * scale;
    const float renderH = virtualH * scale;
    const float offsetX = (screenW - renderW) * 0.5f;
    const float offsetY = (screenH - renderH) * 0.5f;

    Rectangle source = { 0.0f, 0.0f, virtualW, -virtualH }; // Inverted Y for RenderTexture in OpenGL
    Rectangle dest = { offsetX, offsetY, renderW, renderH };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(m_target.texture, source, dest, origin, 0.0f, WHITE);
}

bool VirtualCanvas::saveScreenshot(const std::string& path, int scale) const {
    if (!m_initialized || path.empty()) return false;
    Image img = LoadImageFromTexture(m_target.texture);
    ImageFlipVertical(&img);
    if (scale > 1) {
        ImageResizeNN(&img, Core::GameConfig::VIRTUAL_WIDTH * scale, Core::GameConfig::VIRTUAL_HEIGHT * scale);
    }
    bool ok = ExportImage(img, path.c_str());
    UnloadImage(img);
    return ok;
}

} // namespace Render
