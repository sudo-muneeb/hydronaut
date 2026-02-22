#pragma once
#include "Level.hpp"
#include "Player.hpp"
#include "SineObstacle.hpp"
#include "ParabolicObstacle.hpp"
#include "Treasure.hpp"
#include <vector>

// ─── Level 2 — Arc of Chaos ───────────────────────────────────────────────────
// Sine-wave urchin + parabolic crab. Collect treasure chests to score.
class Level2 : public Level {
public:
    explicit Level2(sf::RenderWindow& window);

    // ─── RL environment API ────────────────────────────────────────────────
    // 12-element normalised state (see implementation_plan for layout).
    std::vector<float> getState()                                   const override;
    std::vector<float> reset(sf::Vector2u windowSize)                     override;
    std::vector<float> step(int action, float& reward, bool& isDone)      override;

protected:
    bool update() override;
    void draw()   override;

private:
    Player             m_player;
    SineObstacle       m_sine;
    ParabolicObstacle  m_para;
    Treasure           m_treasure;
    float              m_prevDistToTreasure = -1.f;  // for distance shaping
};
