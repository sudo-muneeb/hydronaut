#include "SineObstacle.hpp"
#include "AssetManager.hpp"
#include <cstdlib>
#include <cmath>

// Scale sprite to ~8% of window height
static constexpr float SCALE_FACTOR = 0.08f;

SineObstacle::SineObstacle(sf::Vector2u windowSize, const std::string& textureName) {
    m_sprite.setTexture(AssetManager::instance().texture(textureName));
    reset(windowSize);
}

void SineObstacle::reset(sf::Vector2u windowSize) {
    float h    = windowSize.y * SCALE_FACTOR;
    float s    = h / static_cast<float>(m_sprite.getTextureRect().height > 0
                     ? m_sprite.getTextureRect().height : 1);
    m_sprite.setScale(s, s);
    // Start off to the right at a random x in [60%, 100%] of width
    m_x = windowSize.x * 0.6f +
          static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.4f + 1));
    m_sprite.setPosition(m_x, sineY(m_x, windowSize));
}

float SineObstacle::sineY(float x, sf::Vector2u win) const {
    float w = static_cast<float>(win.x);
    float h = static_cast<float>(win.y);
    float half = h * 0.5f;
    if (x <= w * 0.5f)
        return half * (1.f - std::sin(M_PI / (w * 0.5f) * x));
    else
        return half + std::abs(std::sin(M_PI / (w * 0.5f) * (x - w * 0.5f))) * half;
}

void SineObstacle::update(sf::Vector2u windowSize) {
    m_x -= 3.f;
    if (m_x < 0) {
        m_x = windowSize.x * 0.5f +
              static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
    }
    m_sprite.setPosition(m_x, sineY(m_x, windowSize));
}

void SineObstacle::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

sf::FloatRect SineObstacle::getBounds() const {
    return m_sprite.getGlobalBounds();
}
