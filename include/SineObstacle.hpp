#pragma once
#include "Obstacle.hpp"

// ─── Level 2 — Sine-wave sea-urchin obstacle ──────────────────────────────────
// Moves left while tracing a sine wave scaled to window height.
class SineObstacle : public Obstacle {
public:
    SineObstacle(sf::Vector2u windowSize, const std::string& textureName);

    void update(sf::Vector2u windowSize) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    void reset(sf::Vector2u windowSize) override;

private:
    float sineY(float x, sf::Vector2u win) const;

    sf::Sprite m_sprite;
    float      m_x;
};
