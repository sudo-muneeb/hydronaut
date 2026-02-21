#pragma once
#include "Obstacle.hpp"
#include <string>

// ─── Level 3 — Secant-curve octopus obstacle ─────────────────────────────────
// Follows a sec() curve scaled to window dimensions. Moves left very slowly.
class SecObstacle : public Obstacle {
public:
    SecObstacle(sf::Vector2u windowSize, const std::string& textureName);

    void          update(sf::Vector2u windowSize) noexcept override;
    void          draw(sf::RenderWindow& window)  const noexcept override;
    sf::FloatRect getBounds()                     const noexcept override;
    void          reset(sf::Vector2u windowSize)  override;

private:
    float secY(float x, sf::Vector2u win) const noexcept;

    sf::Sprite m_sprite;
    float      m_x;
};
