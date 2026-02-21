#pragma once
#include "Obstacle.hpp"

// ─── Level 3 — Exponential-damped sine lanternfish obstacle ──────────────────
// Follows 100*sin(0.01x)*exp(-0.001x) scaled relative to window height.
class ExpSineObstacle : public Obstacle {
public:
    ExpSineObstacle(sf::Vector2u windowSize, const std::string& textureName);

    void update(sf::Vector2u windowSize) override;
    void draw(sf::RenderWindow& window) const override;
    sf::FloatRect getBounds() const override;
    void reset(sf::Vector2u windowSize) override;

private:
    float expSineY(float x, sf::Vector2u win) const;

    sf::Sprite m_sprite;
    float      m_x;
};
