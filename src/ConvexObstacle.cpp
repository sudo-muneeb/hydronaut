#include "ConvexObstacle.hpp"
#include <cstdlib>
#include <cmath>

// Scale obstacle relative to window: roughly 2% of smaller dimension
static constexpr float OBS_SIZE_FACTOR = 0.02f;

ConvexObstacle::ConvexObstacle(sf::Vector2u windowSize)
    : m_speed(4.f), m_rotation(0.f)
{
    reset(windowSize);
}

void ConvexObstacle::reset(sf::Vector2u windowSize) {
    float size = std::min(windowSize.x, windowSize.y) * OBS_SIZE_FACTOR * 5.f;

    m_shape.setPointCount(3);
    m_shape.setPoint(0, sf::Vector2f(0, 0));
    m_shape.setPoint(1, sf::Vector2f(size / 2.f, size));
    m_shape.setPoint(2, sf::Vector2f(size, 0));
    m_shape.setFillColor(sf::Color(255, 140, 0));
    m_shape.setOutlineColor(sf::Color(255, 200, 50));
    m_shape.setOutlineThickness(1.5f);

    m_x = static_cast<float>(windowSize.x);
    m_y = static_cast<float>(std::rand() % windowSize.y);
    m_shape.setPosition(m_x, m_y);
}

void ConvexObstacle::setSpeed(float speed) { m_speed = speed; }

void ConvexObstacle::update(sf::Vector2u /*windowSize*/) {
    m_x -= m_speed;
    m_rotation += 2.0f;
    m_shape.setPosition(m_x, m_y);
    m_shape.setRotation(m_rotation);
}

void ConvexObstacle::draw(sf::RenderWindow& window) const {
    window.draw(m_shape);
}

sf::FloatRect ConvexObstacle::getBounds() const {
    return m_shape.getGlobalBounds();
}

// ─── Pool ─────────────────────────────────────────────────────────────────────
void ConvexObstaclePool::update(sf::Vector2u windowSize, float speed) {
    m_speed = speed;

    // Random spawning (approx 1-in-50 chance per frame)
    if (std::rand() % 50 == 0)
        spawnOne(windowSize);

    for (auto& obs : m_obstacles)
        obs.update(windowSize);

    // Remove off-screen obstacles
    m_obstacles.erase(
        std::remove_if(m_obstacles.begin(), m_obstacles.end(),
            [](const ConvexObstacle& o) {
                return o.getBounds().left + o.getBounds().width < 0;
            }),
        m_obstacles.end());
}

void ConvexObstaclePool::draw(sf::RenderWindow& window) const {
    for (const auto& obs : m_obstacles)
        obs.draw(window);
}

bool ConvexObstaclePool::collidesWithPlayer(sf::FloatRect playerBounds) const {
    for (const auto& obs : m_obstacles)
        if (playerBounds.intersects(obs.getBounds()))
            return true;
    return false;
}

void ConvexObstaclePool::spawnOne(sf::Vector2u windowSize) {
    ConvexObstacle obs(windowSize);
    obs.setSpeed(m_speed);
    m_obstacles.push_back(std::move(obs));
}
