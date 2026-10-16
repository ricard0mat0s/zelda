#pragma once

#include "core/MapData.hpp"
#include "raylib.h"

namespace Render {

class MapRenderer {
public:
    MapRenderer() = default;

    // Renders the visible ground and obstacle tiles within the camera viewport
    void render(const Core::MapData& map, const Camera2D& camera);
};

} // namespace Render
