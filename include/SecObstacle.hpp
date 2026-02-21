#pragma once
#include "Obstacle.hpp"

// ─── Level 3 — Secant-curve octopus obstacle ─────────────────────────────────
// Follows a sec() curve scaled to window dimensions. Moves left very slowly.
class SecObstacle : public Obstacle {
public:
    SecObstacle(sf::Vector2u windowSize, const std::string& textureName);

    void update(sf::Vector2u windowSize) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    void reset(sf::Vector2u windowSize) override;

private:
    float secY(float x, sf::Vector2u win) const;

    sf::Sprite m_sprite;
    float      m_x;
};
