#include "AssetManager.hpp"
#include "Constants.hpp"
#include <stdexcept>

AssetManager& AssetManager::instance() {
    static AssetManager inst;
    return inst;
}

void AssetManager::loadAll() {
    // ─── Font ──────────────────────────────────────────────────────────────
    if (!m_font.loadFromFile(ASSET_FONT))
        throw std::runtime_error(std::string("Failed to load font: ") + ASSET_FONT);

    // ─── Textures ──────────────────────────────────────────────────────────
    auto load = [&](const std::string& name, const char* path) {
        sf::Texture tex;
        if (!tex.loadFromFile(path))
            throw std::runtime_error(std::string("Failed to load texture: ") + path);
        tex.setSmooth(true);
        m_textures[name] = std::move(tex);
    };

    load("submarine",    ASSET_SUBMARINE);
    load("crab",         ASSET_CRAB);
    load("fish",         ASSET_FISH);
    load("octopus",      ASSET_OCTOPUS);
    load("treasure",     ASSET_TREASURE);
    load("urchin",       ASSET_URCHIN);
}

sf::Font& AssetManager::font() { return m_font; }

sf::Texture& AssetManager::texture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it == m_textures.end())
        throw std::runtime_error("AssetManager: unknown texture '" + name + "'");
    return it->second;
}
