#include "SecObstacle.hpp"
#include "AssetManager.hpp"
#include <cstdlib>
#include <cmath>

static constexpr float SCALE_FACTOR = 0.08f;

SecObstacle::SecObstacle(sf::Vector2u windowSize, const std::string& textureName) {
    m_sprite.setTexture(AssetManager::instance().texture(textureName));
    reset(windowSize);
}

void SecObstacle::reset(sf::Vector2u windowSize) {
    float h = windowSize.y * SCALE_FACTOR;
    float s = h / static_cast<float>(m_sprite.getTextureRect().height > 0
                  ? m_sprite.getTextureRect().height : 1);
    m_sprite.setScale(s, s);
    m_x = windowSize.x * 0.5f +
          static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
    m_sprite.setPosition(m_x, secY(m_x, windowSize));
}

float SecObstacle::secY(float x, sf::Vector2u win) const {
    float w    = static_cast<float>(win.x);
    float h    = static_cast<float>(win.y);
    float half = h * 0.5f;
    // sec curve: cos approaches 0 at odd multiples of pi/2
    float angle = (M_PI / w) * 3.f * x;
    float c     = std::cos(angle);
    if (std::abs(c) < 0.05f) c = 0.05f;  // guard against asymptote
    float val   = (1.f / c) / 6.f;       // matches original formula
    if (val > 0)
        return val * h * 0.5f;
    else
        return h * 0.5f * (1.f - std::abs(val)) + half;
}

void SecObstacle::update(sf::Vector2u windowSize) {
    m_x -= 1.f;
    if (m_x < 0)
        m_x = windowSize.x * 0.5f +
              static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
    m_sprite.setPosition(m_x, secY(m_x, windowSize));
}

void SecObstacle::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

sf::FloatRect SecObstacle::getBounds() const {
    return m_sprite.getGlobalBounds();
}
