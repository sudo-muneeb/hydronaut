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
    if (windowSize.x == 0 || windowSize.y == 0) return;
    float h = windowSize.y * SCALE_FACTOR;
    float s = h / static_cast<float>(m_sprite.getTextureRect().height > 0
                  ? m_sprite.getTextureRect().height : 1);
    m_sprite.setScale(s, s);
    float rangeX = windowSize.x * 0.5f;
    m_x = windowSize.x * 0.5f +
          (rangeX > 0 ? static_cast<float>(std::rand() % static_cast<int>(rangeX)) : 0.f);
    m_sprite.setPosition(m_x, expSineY(m_x, windowSize));
}

float ExpSineObstacle::expSineY(float x, sf::Vector2u win) const noexcept {
    float h         = static_cast<float>(win.y);
    float half      = h * 0.5f;
    float amplitude = h * 0.15f;  // 15% of window height
    // exp(-0.001*x) naturally decays to ~0 for large x; no special guard needed.
    return half + amplitude * std::sin(0.01f * x) * std::exp(-0.001f * x);
}

void ExpSineObstacle::update(sf::Vector2u windowSize) noexcept {
    if (windowSize.x == 0) return;
    m_x -= 3.f * m_speedMult;
    if (m_x < 0) {
        float rangeX = windowSize.x * 0.5f;
        m_x = windowSize.x * 0.5f +
              (rangeX > 0 ? static_cast<float>(std::rand() % static_cast<int>(rangeX)) : 0.f);
    }
    m_sprite.setPosition(m_x, expSineY(m_x, windowSize));
}

void ExpSineObstacle::draw(sf::RenderWindow& window) const noexcept {
    window.draw(m_sprite);
}

sf::FloatRect ExpSineObstacle::getBounds() const noexcept {
    return m_sprite.getGlobalBounds();
}
