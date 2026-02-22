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

    // ─── RL helpers ────────────────────────────────────────────────────
    // Apply a velocity impulse (used by the RL agent to inject actions).
    void applyImpulse(sf::Vector2f delta) noexcept { m_vel += delta; }
    // Centre of the sprite in screen space.
    sf::Vector2f getPosition() const noexcept {
        auto b = getBounds();
        return { b.left + b.width * 0.5f, b.top + b.height * 0.5f };
    }
    // Current velocity (px/frame) for RL state observation.
    sf::Vector2f getVelocity() const noexcept { return m_vel; }
    // Sprite reference for pixel-perfect collision (SpriteBounds.hpp).
    const sf::Sprite& getSprite() const noexcept { return m_sprite; }
    // Update the known window size (needed for boundary clamping on RL step path).
    void setWindowSize(sf::Vector2u ws) noexcept { m_winSize = ws; }

    // ─── Debug ─────────────────────────────────────────────────────────────
    // Draw lethal (red) and graze (yellow) hitbox outlines.
    void drawDebugHitboxes(sf::RenderWindow& window) const;

private:
    void spawnDashTrail(int count);

    sf::Sprite       m_sprite;
    sf::Vector2f     m_vel;
    int              m_dashFrames  = 0;
    sf::Clock        m_dashClock;
    bool             m_dashFired   = false;
    ParticleEmitter  m_particles;
    sf::Vector2u     m_winSize     = {};  // kept in sync each frame for clamping
};
