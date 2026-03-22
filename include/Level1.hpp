#pragma once
#include "SimulationEnvironment.hpp"
#include "HydronautEntity.hpp"
#include "ConvexObstacle.hpp"
#include <vector>

// ─── SimulationEnvironment 1 — Obstacles Unleashed ───────────────────────────────────────────
// Rotating convex triangles spawn from the right. Score increments every frame.
// Speed scales with score.
class Level1 : public SimulationEnvironment {
public:
    explicit Level1(sf::RenderWindow& window);

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
    HydronautEntity   m_player;
    ConvexObstaclePool m_pool;
    int               m_score    = 0;
    float             m_speed    = 4.f;
};
