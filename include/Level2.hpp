#pragma once
#include "Level.hpp"
#include "Player.hpp"
#include "SineObstacle.hpp"
#include "ParabolicObstacle.hpp"
#include "Treasure.hpp"

// ─── Level 2 — Arc of Chaos ───────────────────────────────────────────────────
// Sine-wave urchin + parabolic crab. Collect treasure chests to score.
class Level2 : public Level {
public:
    explicit Level2(sf::RenderWindow& window);

protected:
    bool update() override;
    void draw() override;

private:
    Player             m_player;
    SineObstacle       m_sine;
    ParabolicObstacle  m_para;
    Treasure           m_treasure;
};
