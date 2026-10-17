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

# Add procedural slime enemy sprites
slime_pal = {
    '.': (0, 0, 0, 0),
    'O': (11, 12, 16, 255),
    'D': (27, 67, 50, 255),
    'G': (45, 106, 79, 255),
    'L': (82, 183, 136, 255),
    'H': (149, 213, 178, 255),
    'E': (8, 28, 21, 255),
    'W': (255, 255, 255, 255)
}

slime_0_rows = [
    "................",
    "................",
    "................",
    ".....OOOOOO.....",
    "....OGGGGGGO....",
    "....OGHHHHGGO...",
    "...OGHWWHLWWGO..",
    "...OGHEEHLEEHGO.",
    "..OGGGGGGGGGGGGO",
    "..OGGGGGGGGGGGGO",
    "..OGGDDDDDDDDGGO",
    "..OGDDDDDDDDDDGO",
    "..ODDDDDDDDDDDDO",
    "..OOOOOOOOOOOOOO",
    "................",
    "................"
]

slime_1_rows = [
    "................",
    "................",
    ".....OOOOOO.....",
    "....OGGGGGGO....",
    "....OGHHHHGGO...",
    "...OGHWWHLWWGO..",
    "...OGHEEHLEEHGO.",
    "...OGGGGGGGGGGO.",
    "...OGGGGGGGGGGO.",
    "...OGGDDDDDDGGO.",
    "...OGDDDDDDDDGO.",
    "...ODDDDDDDDDDO.",
    "...OOOOOOOOOOOO.",
    "................",
    "................",
    "................"
]

for sname, srows in [('slime_0', slime_0_rows), ('slime_1', slime_1_rows)]:
    spix = []
    for r in srows:
        for ch in r:
            spix.append(slime_pal.get(ch, (0, 0, 0, 0)))
    assets.append((sname, spix))

# Dungeon sprites
dungeon_pal = {
    '.': (0, 0, 0, 0),
    'B': (15, 23, 42, 255),    # Very dark slate #0f172a
    'D': (30, 41, 59, 255),    # Dark slate #1e293b
    'M': (51, 65, 85, 255),    # Mid slate #334155
    'L': (100, 116, 139, 255), # Light slate #64748b
    'H': (148, 163, 184, 255), # Highlight slate #94a3b8
    'W': (241, 245, 249, 255), # White/near-white #f1f5f9
    'G': (234, 179, 8, 255),   # Gold rune #eab308
    'Y': (250, 204, 21, 255),  # Bright gold #facc15
    'C': (6, 182, 212, 255),   # Cyan glow #06b6d4
    'A': (34, 211, 238, 255),  # Aqua bright #22d3ee
    'I': (71, 85, 105, 255),   # Iron bar #475569
    'O': (2, 6, 23, 255),      # Black/deep void #020617
}

dungeon_floor_rows = [
    "DDDDDDDDDDDDDDDB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "BBBBBBBBBBBBBBBB",
    "DDDDDDDBDDDDDDDB",
    "DMMMMMMBDMMMMMMB",
    "DMMMMMMBDMMMMMMB",
    "DMMMMMMBDMMMMMMB",
    "DMMMMMMBDMMMMMMB",
    "DMMMMMMBDMMMMMMB",
    "DMMMMMMBDMMMMMMB",
    "BBBBBBBBBBBBBBBB",
]

dungeon_wall_rows = [
    "HHHHHHHHHHHHHHHH",
    "HLLLLLLBHLLLLLLB",
    "HLDDDDLBLDDDDDLB",
    "HLDDDDLBLDDDDDLB",
    "BBBBBBBBBBBBBBBB",
    "LLLLLLBLLLLLLBLH",
    "DDDDDLBLDDDDDLBL",
    "DDDDDLBLDDDDDLBL",
    "BBBBBBBBBBBBBBBB",
    "HLLLLLLBHLLLLLLB",
    "HLDDDDLBLDDDDDLB",
    "HLDDDDLBLDDDDDLB",
    "BBBBBBBBBBBBBBBB",
    "ODDDDDDODDDDDDDO",
    "ODDDDDDODDDDDDDO",
    "OOOOOOOOOOOOOOOO",
]

dungeon_door_closed_rows = [
    "HHHHHHHHHHHHHHHH",
    "HLLLLLLLLLLLLLLH",
    "HLDDDDDDDDDDDDLH",
    "HLDOOIOOIOOIOODH",
    "HLDOOIOOIOOIOODH",
    "HLDIIIIIIIIIIIDH",
    "HLDOOIOOIOOIOODH",
    "HLDOOIOOIOOIOODH",
    "HLDIIIIIIIIIIIDH",
    "HLDOOIOOIOOIOODH",
    "HLDOOIOOIOOIOODH",
    "HLDOOIOOIOOIOODH",
    "HLDIIIIIIIIIIIDH",
    "HLDOOIOOIOOIOODH",
    "HLDOOIOOIOOIOODH",
    "BBBBBBBBBBBBBBBB",
]

dungeon_door_open_rows = [
    "HHHHHHHHHHHHHHHH",
    "HLLLLLLLLLLLLLLH",
    "HLDDDDDDDDDDDDLH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "HLDOOOOOOOOOOODH",
    "DDDOOOOOOOOOODDD",
]

pressure_plate_up_rows = [
    "DDDDDDDDDDDDDDDB",
    "DMMMMMMMMMMMMMMB",
    "DMHHHHHHHHHHHMMB",
    "DMHLLLLLLLLLHMMB",
    "DMHLDDDDDDDLHMMB",
    "DMHLDDGGDDDLHMMB",
    "DMHLDGYGYGDLHMMB",
    "DMHLGGYYYYGGLHMB",
    "DMHLGGYYYYGGLHMB",
    "DMHLDGYGYGDLHMMB",
    "DMHLDDGGDDDLHMMB",
    "DMHLDDDDDDDLHMMB",
    "DMHLLLLLLLLLHMMB",
    "DMBBBBBBBBBBBMMB",
    "DMMMMMMMMMMMMMMB",
    "BBBBBBBBBBBBBBBB",
]

pressure_plate_down_rows = [
    "DDDDDDDDDDDDDDDB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "DMLLLLLLLLLLLMMB",
    "DMLDDDDDDDDDLMMB",
    "DMLDDCCDDDDDLMMB",
    "DMLDCACACDDDLMMB",
    "DMLCCAACAACCLMMB",
    "DMLCCAACAACCLMMB",
    "DMLDCACACDDDLMMB",
    "DMLDDCCDDDDDLMMB",
    "DMLDDDDDDDDDLMMB",
    "DMLLLLLLLLLLLMMB",
    "DMMMMMMMMMMMMMMB",
    "DMMMMMMMMMMMMMMB",
    "BBBBBBBBBBBBBBBB",
]

dungeon_stairs_rows = [
    "HHHHHHHHHHHHHHHH",
    "HLLLLLLLLLLLLLLH",
    "HLDDDDDDDDDDDDLH",
    "HBBBBBBBBBBBBBBH",
    "LLLLLLLLLLLLLLLL",
    "LDDDDDDDDDDDDDDL",
    "LBBBBBBBBBBBBBBB",
    "MMMMMMMMMMMMMMMM",
    "MDDDDDDDDDDDDDDM",
    "MBBBBBBBBBBBBBBM",
    "DDDDDDDDDDDDDDDD",
    "DBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBB",
    "OOOOOOOOOOOOOOOO",
    "OOOOOOOOOOOOOOOO",
    "OOOOOOOOOOOOOOOO",
]

dungeon_sprite_list = [
    ('dungeon_floor', dungeon_floor_rows),
    ('dungeon_wall', dungeon_wall_rows),
    ('dungeon_door_closed', dungeon_door_closed_rows),
    ('dungeon_door_open', dungeon_door_open_rows),
    ('pressure_plate_up', pressure_plate_up_rows),
    ('pressure_plate_down', pressure_plate_down_rows),
    ('dungeon_stairs', dungeon_stairs_rows),
]

for dname, drows in dungeon_sprite_list:
    dpix = []
    for r in drows:
        for ch in r:
            dpix.append(dungeon_pal.get(ch, (0, 0, 0, 0)))
    assets.append((dname, dpix))


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
