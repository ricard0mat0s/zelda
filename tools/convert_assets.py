import re

def parse_color(hex_str):
    hex_str = hex_str.strip().lstrip('#')
    if len(hex_str) == 6:
        r = int(hex_str[0:2], 16)
        g = int(hex_str[2:4], 16)
        b = int(hex_str[4:6], 16)
        a = 255
    elif len(hex_str) == 8:
        r = int(hex_str[0:2], 16)
        g = int(hex_str[2:4], 16)
        b = int(hex_str[4:6], 16)
        a = int(hex_str[6:8], 16)
    else:
        r, g, b, a = 0, 0, 0, 0
    return (r, g, b, a)

with open('assets/legacy/AssetGenerator.ts', 'r') as f:
    content = f.read()

pattern = r'(\w+):\s*\{\s*palette:\s*\{([^}]+)\},\s*pixels:\s*\[([^\]]+)\]'
matches = re.findall(pattern, content)

assets = []
for name, pal_str, pix_str in matches:
    pal = {}
    for line in pal_str.strip().split('\n'):
        line = line.split('//')[0].strip().rstrip(',')
        if ':' in line:
            k, v = line.split(':', 1)
            k = k.strip().strip("'\"")
            v = v.strip().strip("'\"")
            pal[k] = parse_color(v)
    
    rows = []
    for line in pix_str.strip().split('\n'):
        line = line.split('//')[0].strip().rstrip(',')
        if line.startswith("'") or line.startswith('"'):
            row = line.strip("'\" ")
            rows.append(row)
    if name in ('hero_right_0', 'hero_right_1'):
        rows[4] = '....OCCCCCEFFO..'
    
    pixels = []
    for r in rows:
        for ch in r:
            color = pal.get(ch, (0, 0, 0, 0))
            pixels.append(color)
    assets.append((name, pixels))

# Write GeneratedAssets.hpp
with open('src/render/GeneratedAssets.hpp', 'w') as f:
    f.write('''#pragma once

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

const std::array<PixelAssetData, ''' + str(len(assets)) + '''>& getAllGeneratedAssets();
const PixelAssetData* findGeneratedAsset(std::string_view name);

} // namespace Render
''')

# Write GeneratedAssets.cpp
with open('src/render/GeneratedAssets.cpp', 'w') as f:
    f.write('''#include "render/GeneratedAssets.hpp"
#include <cstring>

namespace Render {

static const std::array<PixelAssetData, ''' + str(len(assets)) + '''> s_assets = {{
''')
    for name, pixels in assets:
        f.write(f'    {{\n        "{name}",\n        16, 16,\n        {{\n')
        for i in range(0, len(pixels), 8):
            chunk = pixels[i:i+8]
            cols = ', '.join(f'RgbaColor{{{r}, {g}, {b}, {a}}}' for r, g, b, a in chunk)
            f.write(f'            {cols},\n')
        f.write('        }\n    },\n')
    f.write('''}};

const std::array<PixelAssetData, ''' + str(len(assets)) + '''>& getAllGeneratedAssets() {
    return s_assets;
}

const PixelAssetData* findGeneratedAsset(std::string_view name) {
    for (const auto& asset : s_assets) {
        if (name == asset.name) {
            return &asset;
        }
    }
    return nullptr;
}

} // namespace Render
''')

print("GeneratedAssets.hpp and GeneratedAssets.cpp generated successfully!")
