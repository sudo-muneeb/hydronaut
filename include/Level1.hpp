#pragma once
#include "Level.hpp"
#include "Player.hpp"
#include "ConvexObstacle.hpp"
#include <vector>

// ─── Level 1 — Obstacles Unleashed ───────────────────────────────────────────
// Rotating convex triangles spawn from the right. Score increments every frame.
// Speed scales with score.
class Level1 : public Level {
public:
    explicit Level1(sf::RenderWindow& window);

    // ─── RL environment API ────────────────────────────────────────────────
    // 12-element normalised state (see implementation_plan for layout).
    std::vector<float> getState()                                   const override;
    std::vector<float> reset(sf::Vector2u windowSize)                     override;
    std::vector<float> step(int action, float& reward, bool& isDone)      override;

protected:
    bool update() override;
    void draw()   override;

private:
    Player            m_player;
    ConvexObstaclePool m_pool;
    int               m_score    = 0;
    float             m_speed    = 4.f;
};
