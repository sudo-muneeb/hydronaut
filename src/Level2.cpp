#include "Level2.hpp"
#include "Constants.hpp"

Level2::Level2(sf::RenderWindow& window)
    : Level(window)
    , m_player(window.getSize())
    , m_sine(window.getSize(), "urchin")
    , m_para(window.getSize(), "crab")
    , m_treasure(window.getSize())
{
}

bool Level2::update() {
    auto  winSize = m_window.getSize();
    float dt      = 1.f / 60.f;
    float sonar   = getSonarFactor();

    // ─── Player ──────────────────────────────────────────────────────────────
    bool dashed = m_player.handleInput(winSize);
    m_player.update(dt);
    if (dashed) triggerShake(DASH_SHAKE_FRAMES, DASH_SHAKE_INTENSITY);

    // ─── Obstacles (sonar slow) ───────────────────────────────────────────────
    m_sine.setSpeedMultiplier(sonar);
    m_para.setSpeedMultiplier(sonar);
    m_sine.update(winSize);
    m_para.update(winSize);

    // ─── Treasure ────────────────────────────────────────────────────────────
    if (m_player.getBounds().intersects(m_treasure.getBounds())) {
        m_treasure.respawn(winSize);
        addScore(10);
    }

    // ─── Graze (sine) ────────────────────────────────────────────────────────
    int gs = checkGraze(m_player.getGrazeBounds(), m_player.getBounds(),
                        m_sine.getBounds());
    gs    += checkGraze(m_player.getGrazeBounds(), m_player.getBounds(),
                        m_para.getBounds());
    if (gs > 0) addScore(GRAZE_SCORE_PER_FRAME * gs);

    // ─── Lethal collision (iframes during dash) ───────────────────────────────
    if (!m_player.isDashing()) {
        if (m_player.getBounds().intersects(m_sine.getBounds()) ||
            m_player.getBounds().intersects(m_para.getBounds())) {
            triggerShake(SHAKE_FRAMES_DEATH, SHAKE_INTENSITY_DEATH);
            return true;
        }
    }
    return false;
}

void Level2::draw() {
    m_treasure.draw(m_window);
    m_sine.draw(m_window);
    m_para.draw(m_window);
    m_player.draw(m_window);

    if (m_debugMode) {
        m_player.drawDebugHitboxes(m_window);
        // Draw obstacle bounds
        auto drawObs = [&](sf::FloatRect b, sf::Color c) {
            sf::RectangleShape r({b.width, b.height});
            r.setPosition({b.left, b.top});
            r.setFillColor(sf::Color::Transparent);
            r.setOutlineColor(c);
            r.setOutlineThickness(2.f);
            m_window.draw(r);
        };
        drawObs(m_sine.getBounds(), sf::Color(255, 60, 60, 200));
        drawObs(m_para.getBounds(), sf::Color(255, 60, 60, 200));
        // Graze zone of each obstacle for debugging
        auto inflated = [](sf::FloatRect b) {
            b.left  -= 5; b.top  -= 5;
            b.width += 10; b.height += 10;
            return b;
        };
        drawObs(inflated(m_sine.getBounds()), sf::Color(255, 200, 0, 100));
        drawObs(inflated(m_para.getBounds()), sf::Color(255, 200, 0, 100));
    }
}
