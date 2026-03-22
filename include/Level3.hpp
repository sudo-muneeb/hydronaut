#pragma once
#include "SimulationEnvironment.hpp"
#include "HydronautEntity.hpp"
#include "SecObstacle.hpp"
#include "ExpSineObstacle.hpp"
#include "Treasure.hpp"
#include <vector>

// ─── SimulationEnvironment 3 — Waves of Danger ───────────────────────────────────────────────
// Secant-curve octopus + exponential-damped sine lanternfish. Collect treasure.
class Level3 : public SimulationEnvironment {
public:
    explicit Level3(sf::RenderWindow& window);

    // ─── RL environment API ────────────────────────────────────────────────
    // 12-element normalised state (see implementation_plan for layout).
    std::vector<float> getState()                                   const override;
    std::vector<float> reset(sf::Vector2u windowSize)                     override;
    std::vector<float> step(int action, float& reward, bool& isDone)      override;

    const HydronautEntity& getPlayer() const override { return m_player; }
    HydronautEntity&       getPlayer()       override { return m_player; }

    std::unique_ptr<SimulationMemento> create_memento() const override;
    void restore_memento(const SimulationMemento& memento) override;

protected:
    bool update() override;
    void draw()   override;

private:
    HydronautEntity m_player;
    SecObstacle     m_sec;
    ExpSineObstacle m_expSine;
    Treasure        m_treasure;
    float           m_prevDistToTreasure = -1.f;
};
