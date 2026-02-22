#include "Player.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cstdlib>
#include <cmath>

static constexpr float PLAYER_SCALE_FACTOR = 0.07f;

Player::Player(sf::Vector2u windowSize) {
    m_sprite.setTexture(AssetManager::instance().texture("submarine"));
    reset(windowSize);
}

void Player::reset(sf::Vector2u windowSize) {
    auto& tex  = AssetManager::instance().texture("submarine");
    float scale = (windowSize.y * PLAYER_SCALE_FACTOR) /
                  static_cast<float>(tex.getSize().y > 0 ? tex.getSize().y : 1);
    m_sprite.setScale(scale, scale);
    m_sprite.setPosition(windowSize.x * 0.05f, windowSize.y * 0.5f);
    m_vel        = {0.f, 0.f};
    m_dashFrames = 0;
    m_dashFired  = false;
    m_winSize    = windowSize;   // remember for clamping
}

// ─── Input + physics ──────────────────────────────────────────────────────────
bool Player::handleInput(sf::Vector2u windowSize) {
    m_winSize = windowSize;      // keep in sync if window is resized
    bool dashedThisFrame = false;

    // ─── Acceleration from arrow keys ────────────────────────────────────────
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    m_vel.y -= PLAYER_ACCEL;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  m_vel.y += PLAYER_ACCEL;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) m_vel.x += PLAYER_ACCEL;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  m_vel.x -= PLAYER_ACCEL;

    // ─── Hydro-Dash (Space) ───────────────────────────────────────────────────
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && dashAvailable()) {
        float len = std::sqrt(m_vel.x * m_vel.x + m_vel.y * m_vel.y);
        if (len < 0.5f) { m_vel.x = PLAYER_MAX_SPEED; }  // dash right if idle
        m_vel.x *= DASH_VEL_MULTIPLIER;
        m_vel.y *= DASH_VEL_MULTIPLIER;
        m_dashFrames = DASH_IFRAME_FRAMES;
        m_dashFired  = true;
        m_dashClock.restart();
        dashedThisFrame = true;
        spawnDashTrail(12);
    }

    return dashedThisFrame;
}

void Player::update(float dt) noexcept {
    // ─── Water drag (every frame) ─────────────────────────────────────────────
    m_vel.x *= PLAYER_DRAG;
    m_vel.y *= PLAYER_DRAG;

    // ─── Velocity cap (not during dash) ──────────────────────────────────────
    if (m_dashFrames == 0) {
        float len = std::sqrt(m_vel.x * m_vel.x + m_vel.y * m_vel.y);
        if (len > PLAYER_MAX_SPEED) {
            float inv = PLAYER_MAX_SPEED / len;
            m_vel.x  *= inv;
            m_vel.y  *= inv;
        }
    }

    // ─── Move ─────────────────────────────────────────────────────────────────
    m_sprite.move(m_vel);

    // ─── Clamp to window bounds (all 4 edges) ─────────────────────────────────
    if (m_winSize.x > 0 && m_winSize.y > 0) {
        sf::FloatRect b = m_sprite.getGlobalBounds();
        float ww = static_cast<float>(m_winSize.x);
        float wh = static_cast<float>(m_winSize.y);
        float px = m_sprite.getPosition().x;
        float py = m_sprite.getPosition().y;
        // Left / right
        if (b.left < 0.f)                    { px -= b.left;            m_vel.x = 0.f; }
        if (b.left + b.width > ww)           { px -= (b.left + b.width - ww); m_vel.x = 0.f; }
        // Top / bottom
        if (b.top < 0.f)                     { py -= b.top;             m_vel.y = 0.f; }
        if (b.top + b.height > wh)           { py -= (b.top + b.height - wh); m_vel.y = 0.f; }
        m_sprite.setPosition(px, py);
    }

    // ─── Tick dash iframes ────────────────────────────────────────────────────
    if (m_dashFrames > 0) --m_dashFrames;

    // ─── Spawn wake bubbles ───────────────────────────────────────────────────
    sf::FloatRect b = getBounds();
    sf::Vector2f rear(b.left, b.top + b.height * 0.5f);
    m_particles.emit(rear, m_vel, 1);   // 1 per frame; 2 during dash
    if (m_dashFrames > 0)
        m_particles.emit(rear, m_vel, 2);

    m_particles.update(dt);
}

void Player::draw(sf::RenderWindow& window) {
    m_particles.draw(window);

    // ─── Invincibility flash during dash ─────────────────────────────────────
    if (m_dashFrames > 0) {
        sf::Uint8 flash = static_cast<sf::Uint8>(128 + 127 * std::sin(m_dashFrames * 0.8f));
        m_sprite.setColor(sf::Color(flash, 220, 255, 220));
    } else {
        m_sprite.setColor(sf::Color::White);
    }
    window.draw(m_sprite);
}

// ─── Hitboxes ─────────────────────────────────────────────────────────────────
sf::FloatRect Player::getBounds() const noexcept {
    return m_sprite.getGlobalBounds();
}

sf::FloatRect Player::getGrazeBounds() const noexcept {
    sf::FloatRect b = getBounds();
    b.left   -= GRAZE_INFLATE_PX;
    b.top    -= GRAZE_INFLATE_PX;
    b.width  += GRAZE_INFLATE_PX * 2.f;
    b.height += GRAZE_INFLATE_PX * 2.f;
    return b;
}

// ─── Dash state ───────────────────────────────────────────────────────────────
bool Player::dashAvailable() const noexcept {
    if (!m_dashFired) return true;
    return m_dashClock.getElapsedTime().asSeconds() >= DASH_COOLDOWN_SEC;
}

float Player::dashCooldownRemaining() const noexcept {
    if (!m_dashFired) return 0.f;
    float el = m_dashClock.getElapsedTime().asSeconds();
    return std::max(0.f, DASH_COOLDOWN_SEC - el);
}

// ─── Debug helpers ────────────────────────────────────────────────────────────
void Player::drawDebugHitboxes(sf::RenderWindow& window) const {
    // Lethal core (red)
    sf::FloatRect core = getBounds();
    sf::RectangleShape coreRect({core.width, core.height});
    coreRect.setPosition({core.left, core.top});
    coreRect.setFillColor(sf::Color::Transparent);
    coreRect.setOutlineColor(sf::Color(255, 60, 60, 200));
    coreRect.setOutlineThickness(2.f);
    window.draw(coreRect);

    // Graze zone (yellow)
    sf::FloatRect graze = getGrazeBounds();
    sf::RectangleShape grazeRect({graze.width, graze.height});
    grazeRect.setPosition({graze.left, graze.top});
    grazeRect.setFillColor(sf::Color::Transparent);
    grazeRect.setOutlineColor(sf::Color(255, 230, 0, 160));
    grazeRect.setOutlineThickness(1.5f);
    window.draw(grazeRect);
}

// ─── Private ─────────────────────────────────────────────────────────────────
void Player::spawnDashTrail(int count) {
    sf::FloatRect b = getBounds();
    sf::Vector2f  rear(b.left, b.top + b.height * 0.5f);
    m_particles.emit(rear, m_vel, count);
}
