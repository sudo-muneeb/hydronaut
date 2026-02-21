#include "Level1.hpp"
#include "Constants.hpp"
#include <cmath>

Level1::Level1(sf::RenderWindow& window)
    : Level(window), m_player(window.getSize())
{
}

bool Level1::update() {
    auto winSize = m_window.getSize();
    float dt     = 1.f / 60.f;  // fixed step; Level::run() caps at 50ms anyway

    // ─── Player input + physics ───────────────────────────────────────────────
    bool dashed = m_player.handleInput(winSize);
    m_player.update(dt);

    if (dashed) triggerShake(DASH_SHAKE_FRAMES, DASH_SHAKE_INTENSITY);

    // ─── Obstacle speed (sonar slows them) ───────────────────────────────────
    if (getScore() < SCORE_SPEED_THRESHOLD)
        m_speed = INITIAL_OBSTACLE_SPEED *
                  std::pow(SCORE_SPEED_EXPONENT, getScore() / 100.f);

    // Apply sonar slow factor
    float poolSpeed = m_speed * getSonarFactor();
    m_pool.update(winSize, poolSpeed);

    addScore(1);

    // ─── Collision (respect iframes during dash) ─────────────────────────────
    if (!m_player.isDashing()) {
        if (m_pool.collidesWithPlayer(m_player.getBounds())) {
            triggerShake(SHAKE_FRAMES_DEATH, SHAKE_INTENSITY_DEATH);
            return true; // player died
        }
    }

    return false;
}

void Level1::draw() {
    m_pool.draw(m_window);
    m_player.draw(m_window);

    if (m_debugMode) {
        m_player.drawDebugHitboxes(m_window);
        m_pool.drawDebugBounds(m_window);
    }
}
