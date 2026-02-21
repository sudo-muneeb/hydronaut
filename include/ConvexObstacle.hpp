#pragma once
#include "Obstacle.hpp"
#include <vector>

// ─── Level 1 — Rotating convex-triangle obstacle ──────────────────────────────
// Triangles spawn from the right and fly left. Speed scales with score.
// All x/y values are computed relative to window size.
class ConvexObstacle : public Obstacle {
public:
    explicit ConvexObstacle(sf::Vector2u windowSize);

    void          update(sf::Vector2u windowSize) noexcept override;
    void          draw(sf::RenderWindow& window)  const noexcept override;
    sf::FloatRect getBounds()                     const noexcept override;
    void          reset(sf::Vector2u windowSize)  override;   // may throw on zero size

    // Increase speed (called by Level1 as score climbs).
    void setSpeed(float speed) noexcept;

private:
    sf::ConvexShape m_shape;
    float           m_x, m_y;
    float           m_speed;
    float           m_rotation;
};

// ─── Managed collection of ConvexObstacles for Level 1 ───────────────────────
class ConvexObstaclePool {
public:
    void update(sf::Vector2u windowSize, float speed);
    void draw(sf::RenderWindow& window)               const;
    bool collidesWithPlayer(sf::FloatRect playerBounds) const noexcept;

private:
    void spawnOne(sf::Vector2u windowSize);

    std::vector<ConvexObstacle> m_obstacles;
    float m_speed = 4.f;
};
