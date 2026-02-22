#pragma once
#include "Level.hpp"
#include "Player.hpp"
#include "SecObstacle.hpp"
#include "ExpSineObstacle.hpp"
#include "Treasure.hpp"
#include <vector>

// ─── Level 3 — Waves of Danger ───────────────────────────────────────────────
// Secant-curve octopus + exponential-damped sine lanternfish. Collect treasure.
class Level3 : public Level {
public:
    explicit Level3(sf::RenderWindow& window);

    // ─── RL environment API ────────────────────────────────────────────────
    // 12-element normalised state (see implementation_plan for layout).
    std::vector<float> getState()                                   const override;
    std::vector<float> reset(sf::Vector2u windowSize)                     override;
    std::vector<float> step(int action, float& reward, bool& isDone)      override;

protected:
    bool update() override;
    void draw()   override;

private:
    Player          m_player;
    SecObstacle     m_sec;
    ExpSineObstacle m_expSine;
    Treasure        m_treasure;
    float           m_prevDistToTreasure = -1.f;
};
