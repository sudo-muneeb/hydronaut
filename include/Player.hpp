#pragma once
#include <SFML/Graphics.hpp>
#include "Particle.hpp"

// ─── Player submarine ─────────────────────────────────────────────────────────
// Velocity-based physics with water drag, Hydro-Dash (Space), and
// a graze hitbox that is larger than the lethal core hitbox.
class Player {
public:
    explicit Player(sf::Vector2u windowSize);

    // Process keyboard, apply physics, emit wake particles.
    // Returns true if the player just activated a dash this frame.
    bool handleInput(sf::Vector2u windowSize);

    void update(float dt) noexcept;      // apply drag + move
    void draw(sf::RenderWindow& window);  // draw particles then sprite

    // ─── Hitboxes ──────────────────────────────────────────────────────────
    sf::FloatRect getBounds()      const noexcept;  // lethal core
    sf::FloatRect getGrazeBounds() const noexcept;  // larger graze zone

    // ─── Dash state ────────────────────────────────────────────────────────
    bool  isDashing()     const noexcept { return m_dashFrames > 0; }
    bool  dashAvailable() const noexcept;
    float dashCooldownRemaining() const noexcept;

    void reset(sf::Vector2u windowSize);

    // ─── Debug ─────────────────────────────────────────────────────────────
    // Draw lethal (red) and graze (yellow) hitbox outlines.
    void drawDebugHitboxes(sf::RenderWindow& window) const;

private:
    void spawnDashTrail(int count);

    sf::Sprite       m_sprite;
    sf::Vector2f     m_vel;
    int              m_dashFrames  = 0;   // iframes remaining
    sf::Clock        m_dashClock;         // time since last dash fired
    bool             m_dashFired   = false; // was dash used at least once?
    ParticleEmitter  m_particles;
};
