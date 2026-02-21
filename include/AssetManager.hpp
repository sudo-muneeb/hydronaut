#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <unordered_map>
#include <stdexcept>

// ─── Singleton asset manager ──────────────────────────────────────────────────
// Load each texture / font once; retrieve by name string.
class AssetManager {
public:
    static AssetManager& instance();

    // Disable copy/move
    AssetManager(const AssetManager&)            = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // Load all game assets upfront; throws std::runtime_error on failure.
    void loadAll();

    sf::Font&    font();
    sf::Texture& texture(const std::string& name);

private:
    AssetManager() = default;

    sf::Font                                    m_font;
    std::unordered_map<std::string, sf::Texture> m_textures;
};
