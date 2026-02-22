#pragma once
#include <SFML/Graphics.hpp>
#include "Particle.hpp"

// ─── Player submarine ─────────────────────────────────────────────────────────
// Velocity-based physics with water drag and Hydro-Dash.
//
// Command interface (action enum, same as RL agent):
//   0=Up  1=Down  2=Left  3=Right  4=None
//
// The Player is completely agnostic to the input source — human keyboard
// polling (InputHandler) and the RL agent (Level::applyAction) both funnel
// through applyCommand().  UI code never leaks into this class.
class Player {
public:
    explicit Player(sf::Vector2u windowSize);

    // ─── Command interface (called each frame before update()) ─────────────
    // Apply one directional command (0–3) or 4 for no input.
    // Returns true if a dash was activated this frame.
    bool applyCommand(int action, bool dashRequested) noexcept;

    void update(float dt) noexcept;      // apply drag + move + clamp
    void draw(sf::RenderWindow& window);

    // ─── Hitboxes ──────────────────────────────────────────────────────────
    sf::FloatRect getBounds()      const noexcept;  // lethal core
    sf::FloatRect getGrazeBounds() const noexcept;  // larger graze zone

    // ─── Dash state ────────────────────────────────────────────────────────
    bool  isDashing()     const noexcept { return m_dashFrames > 0; }
    bool  dashAvailable() const noexcept;
    float dashCooldownRemaining() const noexcept;

    void reset(sf::Vector2u windowSize);

    // ─── RL helpers ────────────────────────────────────────────────────────
    // Apply a velocity impulse (used by Level::applyAction for the RL path).
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
    void drawDebugHitboxes(sf::RenderWindow& window) const;

private:
    void spawnDashTrail(int count);

    sf::Sprite       m_sprite;
    sf::Vector2f     m_vel;
    int              m_dashFrames  = 0;
    sf::Clock        m_dashClock;
    bool             m_dashFired   = false;
    ParticleEmitter  m_particles;
    sf::Vector2u     m_winSize     = {};
};
