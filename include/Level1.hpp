#pragma once
#include "Level.hpp"
#include "Player.hpp"
#include "ConvexObstacle.hpp"

// ─── Level 1 — Obstacles Unleashed ───────────────────────────────────────────
// Rotating convex triangles spawn from the right. Score increments every frame.
// Speed scales with score.
class Level1 : public Level {
public:
    explicit Level1(sf::RenderWindow& window);

protected:
    bool update() override;
    void draw() override;

private:
    Player            m_player;
    ConvexObstaclePool m_pool;
    int               m_score    = 0;
    float             m_speed    = 4.f;
};
