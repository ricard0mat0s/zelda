#pragma once

#include "raylib.h"
#include <string>
#include <string_view>
#include <unordered_map>

namespace Render {

class AssetManager {
public:
    static AssetManager& instance();

    void init();
    void cleanup();

    Texture2D getTexture(std::string_view name) const;
    bool hasTexture(std::string_view name) const;

    // Export all procedural sprites as PNG to disk
    void exportSpritesToDisk(const std::string& outputDir = "assets/sprites");

private:
    AssetManager() = default;
    ~AssetManager();

    std::unordered_map<std::string, Texture2D> m_textures;
    bool m_initialized = false;
};

} // namespace Render
