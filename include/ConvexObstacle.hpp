#pragma once
#include "Obstacle.hpp"
#include <vector>
#include <limits>

// ─── SimulationEnvironment 1 — Rotating convex-triangle obstacle ──────────────────────────────
class ConvexObstacle : public Obstacle {
public:
    explicit ConvexObstacle(sf::Vector2u windowSize);

    void          update(sf::Vector2u windowSize) noexcept override;
    void          draw(sf::RenderWindow& window)  const noexcept override;
    sf::FloatRect getBounds()                     const noexcept override;
    void          reset(sf::Vector2u windowSize)  override;
    void          setSpeed(float speed)           noexcept;

    // ─── Memento API ──────────────────────────────────────────────────────
    struct Snapshot {
        float x, y;
        float speed;
        // rotation not strictly needed since hitboxes are circles, but good for exact visual match
        float rotation;
    };
    Snapshot saveState() const;
    void     restoreState(const Snapshot& snap, sf::Vector2u windowSize);

private:
    sf::ConvexShape m_shape;
    float           m_x, m_y;
    float           m_speed;
    float           m_rotation;
};

// ─── ObstacleSnapshot — used by the RL state vector ──────────────────────────
struct ObstacleSnapshot {
    sf::Vector2f center;    // screen-space centre of the obstacle
    sf::Vector2f velocity;  // frame-by-frame displacement (px/frame)
};

// ─── Managed collection of ConvexObstacles for SimulationEnvironment 1 ───────────────────────
class ConvexObstaclePool {
public:
    void update(sf::Vector2u windowSize, float speed);
    void draw(sf::RenderWindow& window)               const;
    void drawDebugBounds(sf::RenderWindow& window)    const;
    bool collidesWithPlayer(sf::FloatRect playerBounds) const noexcept;

    // RL: returns up to maxCount snapshots for the active obstacles nearest
    // to playerPos, sorted ascending by squared distance.  Pads with
    // zero-velocity, zero-position entries if fewer are active.
    std::vector<ObstacleSnapshot> getSnapshots(sf::Vector2f playerPos,
                                               std::size_t  maxCount) const noexcept;

    // ─── Memento API ──────────────────────────────────────────────────────
    struct Snapshot {
        std::vector<ConvexObstacle::Snapshot> obstacles;
        float speed;
    };
    Snapshot saveState() const;
    void     restoreState(const Snapshot& snap, sf::Vector2u windowSize);

private:
    void spawnOne(sf::Vector2u windowSize);

    std::vector<ConvexObstacle> m_obstacles;
    float m_speed = 4.f;
};
