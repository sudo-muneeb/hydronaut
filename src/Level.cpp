#include "Level.hpp"
#include "Player.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <cstdlib>
#include <cmath>
#include <string>

// ─── Helper: get the current logical view size ────────────────────────────────
// Always use this instead of m_window.getSize() for world-space calculations.
static sf::Vector2f viewSize(const sf::RenderWindow& w) {
    return w.getView().getSize();
}

Level::Level(sf::RenderWindow& window) : m_window(window) {
    // ── Initialise base view from the ACTUAL current window size, not the
    //    stale default view (which stays 900×900 even after OS resizes).
    auto sz     = m_window.getSize();
    m_baseView  = sf::View(sf::FloatRect(0.f, 0.f,
                                         static_cast<float>(sz.x),
                                         static_cast<float>(sz.y)));
    m_window.setView(m_baseView);

    auto& font = AssetManager::instance().font();

    // Font sizes are set dynamically in drawHUD() relative to view height —
    // just initialise with a default here.
    m_scoreText.setFont(font);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setOutlineColor(sf::Color(0, 0, 80));
    m_scoreText.setOutlineThickness(2.f);

    m_grazeText.setFont(font);
    m_grazeText.setFillColor(sf::Color(255, 230, 60));
    m_grazeText.setOutlineColor(sf::Color(80, 60, 0));
    m_grazeText.setOutlineThickness(1.5f);

    m_pauseText.setFont(font);
    m_pauseText.setFillColor(sf::Color::White);
    m_pauseText.setOutlineColor(sf::Color::Black);
    m_pauseText.setOutlineThickness(3.f);
    m_pauseText.setString("PAUSED\nPress P to resume | ESC to quit");
}

// ─── Main game loop ───────────────────────────────────────────────────────────
int Level::run() {
    sf::Clock frameClock;

    while (m_window.isOpen() && !m_gameOver) {
        handleEvents();
        if (!m_window.isOpen()) break;

        if (m_paused) {
            m_window.clear();
            drawBackground();
            draw();
            showPauseOverlay();
            m_window.display();
            continue;
        }

        float dt = frameClock.restart().asSeconds();
        dt = std::min(dt, 0.05f);

        // ─── Sonar radius update ───────────────────────────────────────────
        if (m_sonarActive) {
            m_sonarRadius += SONAR_EXPAND_SPEED * dt;
            if (m_sonarRadius >= SONAR_MAX_RADIUS) {
                m_sonarActive = false;
                m_sonarRadius = 0.f;
                m_sonarFired  = true;
                m_sonarCooldownClock.restart();
            }
        }

        m_window.clear();
        applyShake();
        drawBackground();

        bool died = update();  // derived class
        draw();                // derived class
        drawSonarRing();
        drawDebugOverlay();
        drawHUD();
        drawGrazeHUD();
        restoreView();
        m_window.display();
        sf::sleep(sf::milliseconds(10));

        if (died) {
            sf::sleep(sf::milliseconds(HIT_STOP_MS));  // hit stop
            m_gameOver = true;
        }
    }

    showGameOver();
    return m_score;
}

// ─── Event handling ───────────────────────────────────────────────────────────
void Level::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
            return;
        }
        if (event.type == sf::Event::Resized) {
            unsigned w = std::max(event.size.width,  (unsigned)MIN_WINDOW_WIDTH);
            unsigned h = std::max(event.size.height, (unsigned)MIN_WINDOW_HEIGHT);
            m_window.setSize(sf::Vector2u(w, h));
            // Keep the view in sync with the physical window
            m_baseView = sf::View(sf::FloatRect(0.f, 0.f, (float)w, (float)h));
            m_window.setView(m_baseView);
        }
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::P && !m_pKeyDown) {
            m_paused   = !m_paused;
            m_pKeyDown = true;
        }
        if (event.type == sf::Event::KeyReleased &&
            event.key.code == sf::Keyboard::P)
            m_pKeyDown = false;

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Escape)
            m_gameOver = true;

        // Sonar pulse (X)
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::X && !m_xKeyDown) {
            bool canFire = !m_sonarActive &&
                           (!m_sonarFired ||
                            m_sonarCooldownClock.getElapsedTime().asSeconds() >= SONAR_COOLDOWN_SEC);
            if (canFire) {
                m_sonarActive = true;
                m_sonarRadius = 0.f;
                // Centre sonar on current view centre (logical coords)
                m_sonarCenter = m_baseView.getCenter();
            }
            m_xKeyDown = true;
        }
        if (event.type == sf::Event::KeyReleased &&
            event.key.code == sf::Keyboard::X)
            m_xKeyDown = false;

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::F3)
            m_debugMode = !m_debugMode;
    }
}

// ─── Screen shake ─────────────────────────────────────────────────────────────
void Level::triggerShake(int frames, float intensity) noexcept {
    m_shakeFrames    = frames;
    m_shakeIntensity = intensity;
}

// ─── Inference rendering ────────────────────────────────────────────────
// Renders one visual frame (background, sprites, sonar, HUD) without calling
// update() or polling input.  Mirrors the render block inside run().
void Level::renderFrame() {
    m_window.clear();
    applyShake();
    drawBackground();
    draw();              // virtual — actual level sprites
    drawSonarRing();
    drawDebugOverlay();
    drawHUD();
    drawGrazeHUD();
    restoreView();
    m_window.display();
}

// ─── Virtual size ─────────────────────────────────────────────────────────────
// Overrides the physics/state coordinate space without resizing the OS window.
void Level::setVirtualSize(sf::Vector2u sz) noexcept {
    m_virtualSize = sz;
    m_baseView = sf::View(sf::FloatRect(
        0.f, 0.f,
        static_cast<float>(sz.x), static_cast<float>(sz.y)));
    m_window.setView(m_baseView);
}

sf::Vector2u Level::getSimSize() const noexcept {
    return (m_virtualSize.x > 0 && m_virtualSize.y > 0)
               ? m_virtualSize
               : m_window.getSize();
}


// Action space: 0=Up, 1=Down, 2=Left, 3=Right.
// Applies a fixed velocity impulse equal to PLAYER_ACCEL * 4 so the agent
// responds at a similar scale to human key-holds.
void Level::applyAction(Player& player, int action) noexcept {
    constexpr float IMPULSE = PLAYER_ACCEL * 4.f;
    switch (action) {
        case 0: player.applyImpulse({ 0.f, -IMPULSE}); break;  // Up
        case 1: player.applyImpulse({ 0.f,  IMPULSE}); break;  // Down
        case 2: player.applyImpulse({-IMPULSE, 0.f}); break;   // Left
        case 3: player.applyImpulse({ IMPULSE, 0.f}); break;   // Right
        default: break;
    }
}

void Level::applyShake() {
    if (m_shakeFrames <= 0) {
        m_window.setView(m_baseView);
        return;
    }
    --m_shakeFrames;
    float ox = (static_cast<float>(std::rand() % 100) / 50.f - 1.f) * m_shakeIntensity;
    float oy = (static_cast<float>(std::rand() % 100) / 50.f - 1.f) * m_shakeIntensity;
    sf::View sv = m_baseView;
    sv.setCenter(sv.getCenter() + sf::Vector2f(ox, oy));
    m_window.setView(sv);
}

void Level::restoreView() {
    m_window.setView(m_baseView);
}

// ─── Sonar ───────────────────────────────────────────────────────────────────
float Level::getSonarFactor() const noexcept {
    return m_sonarActive ? SONAR_SLOW_FACTOR : 1.0f;
}

void Level::drawSonarRing() {
    if (!m_sonarActive || m_sonarRadius <= 0.f) return;
    float alpha = 255.f * (1.f - m_sonarRadius / SONAR_MAX_RADIUS);

    sf::CircleShape ring(m_sonarRadius);
    ring.setOrigin(m_sonarRadius, m_sonarRadius);
    ring.setPosition(m_sonarCenter);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineColor(sf::Color(80, 220, 255, static_cast<sf::Uint8>(alpha)));
    ring.setOutlineThickness(3.f);
    m_window.draw(ring);

    if (m_sonarRadius > 20.f) {
        sf::CircleShape inner(m_sonarRadius - 12.f);
        inner.setOrigin(m_sonarRadius - 12.f, m_sonarRadius - 12.f);
        inner.setPosition(m_sonarCenter);
        inner.setFillColor(sf::Color::Transparent);
        inner.setOutlineColor(sf::Color(40, 160, 255, static_cast<sf::Uint8>(alpha * 0.4f)));
        inner.setOutlineThickness(1.5f);
        m_window.draw(inner);
    }
}

// ─── Graze ────────────────────────────────────────────────────────────────────
int Level::checkGraze(sf::FloatRect playerGraze,
                       sf::FloatRect playerCore,
                       sf::FloatRect obstacleBox) const noexcept {
    if (playerCore.intersects(obstacleBox))  return 0;
    if (playerGraze.intersects(obstacleBox)) return 1;
    return 0;
}

void Level::addScore(int amount) noexcept {
    m_score    += amount;
    m_grazeAcc += (amount <= GRAZE_SCORE_PER_FRAME * 2) ? amount : 0;
}

// ─── Background ───────────────────────────────────────────────────────────────
// Uses view size (logical coords), NOT physical window.getSize().
void Level::drawBackground() {
    sf::Vector2f vs = viewSize(m_window);
    sf::VertexArray bg(sf::Quads, 4);
    bg[0] = sf::Vertex(sf::Vector2f(0,     0    ), sf::Color(0,  15,  60));
    bg[1] = sf::Vertex(sf::Vector2f(vs.x,  0    ), sf::Color(0,  15,  60));
    bg[2] = sf::Vertex(sf::Vector2f(vs.x,  vs.y ), sf::Color(0,  60, 120));
    bg[3] = sf::Vertex(sf::Vector2f(0,     vs.y ), sf::Color(0,  60, 120));
    m_window.draw(bg);
}

// ─── HUD ─────────────────────────────────────────────────────────────────────
// All sizes and positions are proportional to the current VIEW size.
void Level::drawHUD() {
    sf::Vector2f vs    = viewSize(m_window);
    float        hScale = vs.y / 900.f;   // scale factor relative to 900-unit reference

    auto& font = AssetManager::instance().font();

    // Score
    unsigned scoreSize = static_cast<unsigned>(28.f * hScale);
    m_scoreText.setCharacterSize(std::max(14u, scoreSize));
    m_scoreText.setString("Score: " + std::to_string(m_score));
    m_scoreText.setPosition(vs.x * 0.012f, vs.y * 0.012f);
    m_window.draw(m_scoreText);

    // Sonar status (below score)
    unsigned smallSize = static_cast<unsigned>(20.f * hScale);
    smallSize = std::max(11u, smallSize);

    sf::Text sonarText;
    sonarText.setFont(font);
    sonarText.setCharacterSize(smallSize);

    bool sonarReady = !m_sonarActive &&
                      (!m_sonarFired ||
                       m_sonarCooldownClock.getElapsedTime().asSeconds() >= SONAR_COOLDOWN_SEC);
    if (sonarReady) {
        sonarText.setFillColor(sf::Color(80, 255, 200, 210));
        sonarText.setString("X Sonar: READY");
    } else if (m_sonarFired && !m_sonarActive) {
        float rem = SONAR_COOLDOWN_SEC - m_sonarCooldownClock.getElapsedTime().asSeconds();
        sonarText.setFillColor(sf::Color(80, 200, 255, 210));
        sonarText.setString("X Sonar: " + std::to_string(static_cast<int>(rem) + 1) + "s");
    } else {
        sonarText.setFillColor(sf::Color(80, 200, 255, 255));
        sonarText.setString("X Sonar: ACTIVE");
    }
    sonarText.setPosition(vs.x * 0.012f, vs.y * 0.012f + scoreSize + 4.f);
    m_window.draw(sonarText);

    // F3 debug hint — top-right corner
    sf::Text dbgHint;
    dbgHint.setFont(font);
    dbgHint.setCharacterSize(std::max(11u, static_cast<unsigned>(14.f * hScale)));
    if (m_debugMode) {
        dbgHint.setFillColor(sf::Color(255, 80, 80, 220));
        dbgHint.setString("DEBUG  [F3 off]");
    } else {
        dbgHint.setFillColor(sf::Color(120, 180, 180, 130));
        dbgHint.setString("[F3] debug");
    }
    sf::FloatRect db = dbgHint.getLocalBounds();
    dbgHint.setOrigin(db.width, 0.f);
    dbgHint.setPosition(vs.x - vs.x * 0.012f, vs.y * 0.012f);
    m_window.draw(dbgHint);

    // Controls reminder — bottom-centre
    sf::Text ctrl;
    ctrl.setFont(font);
    ctrl.setCharacterSize(std::max(11u, static_cast<unsigned>(14.f * hScale)));
    ctrl.setFillColor(sf::Color(120, 180, 220, 110));
    ctrl.setString("SPACE: dash   X: sonar   P: pause   ESC: menu");
    sf::FloatRect cb = ctrl.getLocalBounds();
    ctrl.setOrigin(cb.width / 2.f, cb.height);
    ctrl.setPosition(vs.x / 2.f, vs.y - vs.y * 0.012f);
    m_window.draw(ctrl);
}

void Level::drawGrazeHUD() {
    if (m_grazeAcc == 0) return;
    sf::Vector2f vs    = viewSize(m_window);
    float        hScale = vs.y / 900.f;
    m_grazeText.setCharacterSize(std::max(13u, static_cast<unsigned>(20.f * hScale)));
    m_grazeText.setString("GRAZE  +" + std::to_string(m_grazeAcc));
    sf::FloatRect gb = m_grazeText.getLocalBounds();
    m_grazeText.setOrigin(gb.width / 2.f, 0.f);
    m_grazeText.setPosition(vs.x / 2.f, vs.y * 0.012f);
    m_window.draw(m_grazeText);
}

// ─── Debug overlay ────────────────────────────────────────────────────────────
void Level::drawDebugOverlay() {
    // Hitbox drawing is delegated to Player::drawDebugHitboxes() and
    // per-level obstacle debug code, called from the derived draw().
    // Nothing extra needed here.
}

// ─── Pause overlay ────────────────────────────────────────────────────────────
void Level::showPauseOverlay() {
    sf::Vector2f vs    = viewSize(m_window);
    float        hScale = vs.y / 900.f;

    sf::RectangleShape dim(vs);
    dim.setFillColor(sf::Color(0, 0, 0, 160));
    m_window.draw(dim);

    m_pauseText.setCharacterSize(std::max(24u, static_cast<unsigned>(52.f * hScale)));
    sf::FloatRect pb = m_pauseText.getLocalBounds();
    m_pauseText.setOrigin(pb.width / 2.f, pb.height / 2.f);
    m_pauseText.setPosition(vs.x / 2.f, vs.y / 2.f);
    m_window.draw(m_pauseText);
    m_window.display();
}

// ─── Game over ────────────────────────────────────────────────────────────────
void Level::showGameOver() {
    if (!m_window.isOpen()) return;
    triggerShake(SHAKE_FRAMES_DEATH, SHAKE_INTENSITY_DEATH);

    auto&    font   = AssetManager::instance().font();
    sf::Text goText;
    goText.setFont(font);
    goText.setFillColor(sf::Color(255, 80, 80));
    goText.setOutlineColor(sf::Color::Black);
    goText.setOutlineThickness(3.f);
    goText.setString("Game Over!\nScore: " + std::to_string(m_score) +
                     "\nGraze bonus: " + std::to_string(m_grazeAcc));

    for (int frame = 0; frame < 120; ++frame) {
        sf::Event e;
        while (m_window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { m_window.close(); return; }
        }
        sf::Vector2f vs    = viewSize(m_window);
        float        hScale = vs.y / 900.f;
        goText.setCharacterSize(std::max(24u, static_cast<unsigned>(52.f * hScale)));

        m_window.clear();
        applyShake();
        drawBackground();
        sf::FloatRect tb = goText.getLocalBounds();
        goText.setOrigin(tb.width / 2.f, tb.height / 2.f);
        goText.setPosition(vs.x / 2.f, vs.y / 2.f);
        m_window.draw(goText);
        restoreView();
        m_window.display();
        sf::sleep(sf::milliseconds(16));
    }
}
