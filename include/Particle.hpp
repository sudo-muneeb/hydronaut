#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// ─── Single particle ──────────────────────────────────────────────────────────
struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float        alphaf;     // 0.0–255.0
    float        life;       // seconds remaining
    float        radius;
};

// ─── Particle emitter owned by the Player ────────────────────────────────────
// Spawns bubble/wake particles at the submarine rear.
// Updated and drawn every frame via the Level draw step.
class ParticleEmitter {
public:
    // Spawn N bubbles at 'origin' with a base drift velocity.
    void emit(sf::Vector2f origin, sf::Vector2f baseVel, int count = 1);

    // dt in seconds (1/60).
    void update(float dt) noexcept;

    void draw(sf::RenderWindow& window) const;

    bool empty() const noexcept { return m_particles.empty(); }

private:
    std::vector<Particle> m_particles;
};
