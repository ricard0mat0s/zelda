#include "render/AssetManager.hpp"
#include "render/GeneratedAssets.hpp"
#include <iostream>
#include <filesystem>

namespace Render {

AssetManager& AssetManager::instance() {
    static AssetManager s_instance;
    return s_instance;
}

AssetManager::~AssetManager() {
    cleanup();
}

void AssetManager::init() {
    if (m_initialized) return;

    const auto& assets = getAllGeneratedAssets();
    for (const auto& asset : assets) {
        Image img = GenImageColor(asset.width, asset.height, BLANK);
        for (int y = 0; y < asset.height; ++y) {
            for (int x = 0; x < asset.width; ++x) {
                const auto& p = asset.pixels[y * asset.width + x];
                Color c = { p.r, p.g, p.b, p.a };
                ImageDrawPixel(&img, x, y, c);
            }
        }

        Texture2D tex = LoadTextureFromImage(img);
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
        m_textures[std::string(asset.name)] = tex;

        UnloadImage(img);
    }

    m_initialized = true;
    std::cout << "[AssetManager] Loaded " << m_textures.size() << " procedural textures." << std::endl;
}

void AssetManager::cleanup() {
    if (!m_initialized) return;

    for (auto& [name, tex] : m_textures) {
        UnloadTexture(tex);
    }
    m_textures.clear();
    m_initialized = false;
}

Texture2D AssetManager::getTexture(std::string_view name) const {
    auto it = m_textures.find(std::string(name));
    if (it != m_textures.end()) {
        return it->second;
    }
    std::cerr << "[AssetManager] WARNING: Texture not found: " << name << std::endl;
    return Texture2D{};
}

bool AssetManager::hasTexture(std::string_view name) const {
    return m_textures.find(std::string(name)) != m_textures.end();
}

void AssetManager::exportSpritesToDisk(const std::string& outputDir) {
    std::filesystem::create_directories(outputDir);

    const auto& assets = getAllGeneratedAssets();
    for (const auto& asset : assets) {
        Image img = GenImageColor(asset.width, asset.height, BLANK);
        for (int y = 0; y < asset.height; ++y) {
            for (int x = 0; x < asset.width; ++x) {
                const auto& p = asset.pixels[y * asset.width + x];
                Color c = { p.r, p.g, p.b, p.a };
                ImageDrawPixel(&img, x, y, c);
            }
        }

        std::string filePath = outputDir + "/" + std::string(asset.name) + ".png";
        ExportImage(img, filePath.c_str());
        UnloadImage(img);
    }
    std::cout << "[AssetManager] Exported " << assets.size() << " sprite PNGs to " << outputDir << std::endl;
}

} // namespace Render
