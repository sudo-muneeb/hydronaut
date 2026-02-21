#pragma once
#include <SFML/Graphics.hpp>

// ─── Abstract Obstacle base ──────────────────────────────────────────────────
// All obstacle types implement update() and draw() with respect to the
// live window dimensions so they stay proportionally placed on any resolution.
class Obstacle {
public:
    virtual ~Obstacle() = default;

    // Called once per frame. windowSize drives all position math.
    virtual void update(sf::Vector2u windowSize) = 0;

    virtual void draw(sf::RenderWindow& window) const = 0;

    // Axis-aligned bounding box for collision detection.
    virtual sf::FloatRect getBounds() const = 0;

    // Reset obstacle to its initial off-screen position.
    virtual void reset(sf::Vector2u windowSize) = 0;
};
