#pragma once
#include <SFML/Graphics.hpp>

// ─── Abstract Obstacle base ──────────────────────────────────────────────────
// All obstacle types implement update() and draw() with respect to
// the live window dimensions. Speed can be scaled by setSpeedMultiplier()
// (used by the Sonar Pulse to slow obstacles within its radius).
class Obstacle {
public:
    virtual ~Obstacle() = default;

    virtual void update(sf::Vector2u windowSize) noexcept = 0;
    virtual void draw(sf::RenderWindow& window)  const noexcept = 0;
    virtual sf::FloatRect getBounds()            const noexcept = 0;
    virtual void reset(sf::Vector2u windowSize)  = 0;

    // ─── Sonar slow: sets a [0..1] multiplier applied to movement speed.
    void setSpeedMultiplier(float m) noexcept {
        m_speedMult = (m < 0.f) ? 0.f : (m > 1.f ? 1.f : m);
    }

protected:
    float m_speedMult = 1.0f;
};
