#pragma once

#include <string_view>
#include <array>
#include <cstdint>

namespace Render {

struct RgbaColor {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

struct PixelAssetData {
    const char* name;
    int width = 16;
    int height = 16;
    std::array<RgbaColor, 256> pixels;
};

const std::array<PixelAssetData, 24>& getAllGeneratedAssets();
const PixelAssetData* findGeneratedAsset(std::string_view name);

} // namespace Render
