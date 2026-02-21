#include "Treasure.hpp"
#include "AssetManager.hpp"
#include <cstdlib>

// Keep treasure within [15%, 85%] of each dimension so it's always visible
static constexpr float MARGIN = 0.15f;
static constexpr float SCALE_FACTOR = 0.07f;

Treasure::Treasure(sf::Vector2u windowSize) {
    m_sprite.setTexture(AssetManager::instance().texture("treasure"));
    respawn(windowSize);
}

void Treasure::respawn(sf::Vector2u windowSize) {
    float h = windowSize.y * SCALE_FACTOR;
    auto& tex = AssetManager::instance().texture("treasure");
    float s = h / static_cast<float>(tex.getSize().y);
    m_sprite.setScale(s, s);

    float minX = windowSize.x * MARGIN;
    float maxX = windowSize.x * (1.f - MARGIN) - m_sprite.getGlobalBounds().width;
    float minY = windowSize.y * MARGIN;
    float maxY = windowSize.y * (1.f - MARGIN) - m_sprite.getGlobalBounds().height;

    float rx = minX + static_cast<float>(std::rand() % static_cast<int>(maxX - minX + 1));
    float ry = minY + static_cast<float>(std::rand() % static_cast<int>(maxY - minY + 1));
    m_sprite.setPosition(rx, ry);
}

void Treasure::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

sf::FloatRect Treasure::getBounds() const {
    return m_sprite.getGlobalBounds();
}
