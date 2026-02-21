#include "Level3.hpp"
#include "Constants.hpp"

Level3::Level3(sf::RenderWindow& window)
    : Level(window)
    , m_player(window.getSize())
    , m_sec(window.getSize(), "octopus")
    , m_expSine(window.getSize(), "fish")
    , m_treasure(window.getSize())
{
}

bool Level3::update() {
    auto  winSize = m_window.getSize();
    float dt      = 1.f / 60.f;
    float sonar   = getSonarFactor();

    // ─── Player ──────────────────────────────────────────────────────────────
    bool dashed = m_player.handleInput(winSize);
    m_player.update(dt);
    if (dashed) triggerShake(DASH_SHAKE_FRAMES, DASH_SHAKE_INTENSITY);

    // ─── Obstacles (sonar slow) ───────────────────────────────────────────────
    m_sec.setSpeedMultiplier(sonar);
    m_expSine.setSpeedMultiplier(sonar);
    m_sec.update(winSize);
    m_expSine.update(winSize);

    // ─── Treasure ────────────────────────────────────────────────────────────
    if (m_player.getBounds().intersects(m_treasure.getBounds())) {
        m_treasure.respawn(winSize);
        addScore(10);
    }

    // ─── Graze ───────────────────────────────────────────────────────────────
    int gs = checkGraze(m_player.getGrazeBounds(), m_player.getBounds(),
                        m_sec.getBounds());
    gs    += checkGraze(m_player.getGrazeBounds(), m_player.getBounds(),
                        m_expSine.getBounds());
    if (gs > 0) addScore(GRAZE_SCORE_PER_FRAME * gs);

    // ─── Lethal collision (iframes during dash) ───────────────────────────────
    if (!m_player.isDashing()) {
        if (m_player.getBounds().intersects(m_sec.getBounds()) ||
            m_player.getBounds().intersects(m_expSine.getBounds())) {
            triggerShake(SHAKE_FRAMES_DEATH, SHAKE_INTENSITY_DEATH);
            return true;
        }
    }
    return false;
}

void Level3::draw() {
    m_treasure.draw(m_window);
    m_sec.draw(m_window);
    m_expSine.draw(m_window);
    m_player.draw(m_window);

    if (m_debugMode) {
        m_player.drawDebugHitboxes(m_window);
        auto drawObs = [&](sf::FloatRect b, sf::Color c) {
            sf::RectangleShape r({b.width, b.height});
            r.setPosition({b.left, b.top});
            r.setFillColor(sf::Color::Transparent);
            r.setOutlineColor(c);
            r.setOutlineThickness(2.f);
            m_window.draw(r);
        };
        drawObs(m_sec.getBounds(),     sf::Color(255, 60, 60, 200));
        drawObs(m_expSine.getBounds(), sf::Color(255, 60, 60, 200));
    }
}
