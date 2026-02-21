#pragma once
#include "Obstacle.hpp"
#include <string>

// ─── Level 2 — Parabolic crab obstacle ───────────────────────────────────────
// Bounces between left/right edges following a scaled parabolic arc.
class ParabolicObstacle : public Obstacle {
public:
    ParabolicObstacle(sf::Vector2u windowSize, const std::string& textureName);

    void          update(sf::Vector2u windowSize) noexcept override;
    void          draw(sf::RenderWindow& window)  const noexcept override;
    sf::FloatRect getBounds()                     const noexcept override;
    void          reset(sf::Vector2u windowSize)  override;

private:
    float parabolicY(float x, sf::Vector2u win) const noexcept;

    sf::Sprite m_sprite;
    float      m_x;
    bool       m_movingLeft; // true = moving towards x=0
};
