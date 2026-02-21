#pragma once
#include "Obstacle.hpp"

// ─── Level 2 — Parabolic crab obstacle ───────────────────────────────────────
// Bounces between left/right edges following a scaled parabolic arc.
class ParabolicObstacle : public Obstacle {
public:
    ParabolicObstacle(sf::Vector2u windowSize, const std::string& textureName);

    void update(sf::Vector2u windowSize) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    void reset(sf::Vector2u windowSize) override;

private:
    float parabolicY(float x, sf::Vector2u win) const;

    sf::Sprite m_sprite;
    float      m_x;
    bool       m_movingLeft; // true = moving towards x=0
};
