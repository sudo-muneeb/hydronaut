#include "ParabolicObstacle.hpp"
#include "AssetManager.hpp"
#include <cstdlib>
#include <cmath>

static constexpr float SCALE_FACTOR = 0.08f;

ParabolicObstacle::ParabolicObstacle(sf::Vector2u windowSize, const std::string& textureName)
    : m_movingLeft(true)
{
    m_sprite.setTexture(AssetManager::instance().texture(textureName));
    reset(windowSize);
}

void ParabolicObstacle::reset(sf::Vector2u windowSize) {
    float h = windowSize.y * SCALE_FACTOR;
    float s = h / static_cast<float>(m_sprite.getTextureRect().height > 0
                  ? m_sprite.getTextureRect().height : 1);
    m_sprite.setScale(s, s);
    m_x          = windowSize.x * 0.5f +
                   static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
    m_movingLeft = true;
    m_sprite.setPosition(m_x, parabolicY(m_x, windowSize));
}

float ParabolicObstacle::parabolicY(float x, sf::Vector2u win) const {
    float w    = static_cast<float>(win.x);
    float h    = static_cast<float>(win.y);
    float half = h * 0.5f;
    // Parabola scaled so that at x=0 and x=w the value is ~half ± half
    float norm = x / w;   // 0..1
    float para = 4.f * norm * (1.f - norm);  // peaks at 1 when norm=0.5
    if (m_movingLeft)
        return half - half * std::sqrt(std::max(0.f, para));
    else
        return half + half * std::sqrt(std::max(0.f, para));
}

void ParabolicObstacle::update(sf::Vector2u windowSize) {
    float speed = 4.f;
    if (m_movingLeft) {
        m_x -= speed;
        if (m_x < 0) m_movingLeft = false;
    } else {
        m_x += speed;
        if (m_x > windowSize.x) {
            m_movingLeft = true;
            m_x = windowSize.x * 0.5f +
                  static_cast<float>(std::rand() % static_cast<int>(windowSize.x * 0.5f + 1));
        }
    }
    m_sprite.setPosition(m_x, parabolicY(m_x, windowSize));
}

void ParabolicObstacle::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

sf::FloatRect ParabolicObstacle::getBounds() const {
    return m_sprite.getGlobalBounds();
}
