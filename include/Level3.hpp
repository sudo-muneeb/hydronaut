#pragma once
#include "Level.hpp"
#include "Player.hpp"
#include "SecObstacle.hpp"
#include "ExpSineObstacle.hpp"
#include "Treasure.hpp"

// ─── Level 3 — Waves of Danger ───────────────────────────────────────────────
// Secant-curve octopus + exponential-damped sine lanternfish. Collect treasure.
class Level3 : public Level {
public:
    explicit Level3(sf::RenderWindow& window);

protected:
    bool update() override;
    void draw() override;

private:
    Player          m_player;
    SecObstacle     m_sec;
    ExpSineObstacle m_expSine;
    Treasure        m_treasure;
};
