#include "ExpSineObstacle.hpp"
#include "AssetManager.hpp"
#include <cstdlib>
#include <cmath>

static constexpr float SCALE_FACTOR = 0.08f;

ExpSineObstacle::ExpSineObstacle(sf::Vector2u windowSize, const std::string& textureName) {
    m_sprite.setTexture(AssetManager::instance().texture(textureName));
    reset(windowSize);
}

void ExpSineObstacle::reset(sf::Vector2u windowSize) {
    float h = windowSize.y * SCALE_FACTOR;
    float s = h / static_cast<float>(m_sprite.getTextureRect().height > 0
                  ? m_sprite.getTextureRect().height : 1);
    m_sprite.setScale(s, s);
    m_x = windowSize.x * 0.5f +
          static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
    m_sprite.setPosition(m_x, expSineY(m_x, windowSize));
}

float ExpSineObstacle::expSineY(float x, sf::Vector2u win) const {
    float h    = static_cast<float>(win.y);
    float half = h * 0.5f;
    // Original: HEIGHT/2 + 100*sin(0.01*x)*exp(-0.001*x)
    // Generalized: half + half*0.2*sin(0.01*x)*exp(-0.001*x)
    float amplitude = h * 0.15f;  // scales with window height
    return half + amplitude * std::sin(0.01f * x) * std::exp(-0.001f * x);
}

void ExpSineObstacle::update(sf::Vector2u windowSize) {
    m_x -= 3.f;
    if (m_x < 0)
        m_x = windowSize.x * 0.5f +
              static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
    m_sprite.setPosition(m_x, expSineY(m_x, windowSize));
}

void ExpSineObstacle::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

sf::FloatRect ExpSineObstacle::getBounds() const {
    return m_sprite.getGlobalBounds();
}
