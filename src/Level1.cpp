#include "Level1.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cmath>

// ─── Shared getState() constants ─────────────────────────────────────────────
static constexpr int   MAX_OBJ_SLOTS  = 8;   // object slots in state vector
static constexpr int   NUM_OBJ_TYPES  = 6;   // type IDs 1-6 (0 = empty)
static constexpr float MAX_VEL        = 10.f; // px/frame cap for normalisation
static constexpr float REF_W          = 1920.f;
static constexpr float REF_H          = 1080.f;

Level1::Level1(sf::RenderWindow& window)
    : Level(window), m_player(window.getSize())
{
}

// ─── Normal gameplay update ───────────────────────────────────────────────────
bool Level1::update() {
    auto winSize = m_window.getSize();
    float dt     = 1.f / 60.f;

    bool dashed = m_player.handleInput(winSize);
    m_player.update(dt);
    if (dashed) triggerShake(DASH_SHAKE_FRAMES, DASH_SHAKE_INTENSITY);

    if (getScore() < SCORE_SPEED_THRESHOLD)
        m_speed = INITIAL_OBSTACLE_SPEED *
                  std::pow(SCORE_SPEED_EXPONENT, getScore() / 100.f);

    float poolSpeed = m_speed * getSonarFactor();
    m_pool.update(winSize, poolSpeed);
    addScore(1);

    if (!m_player.isDashing()) {
        if (m_pool.collidesWithPlayer(m_player.getBounds())) {
            triggerShake(SHAKE_FRAMES_DEATH, SHAKE_INTENSITY_DEATH);
            return true;
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

// ─── RL Environment API ───────────────────────────────────────────────────────
// Generic 49-element state:
//   [0..8]   Header  (level, screen, player pos+vel, reward timing)
//   [9..48]  8 × 5  Object slots (type_id, x, y, vx, vy)
//
// Level 1: type 2 = convex triangle; no treasure; slots padded with zeros.
std::vector<float> Level1::getState() const {
    auto winSize = m_window.getSize();
    float wf = static_cast<float>(winSize.x ? winSize.x : 1);
    float hf = static_cast<float>(winSize.y ? winSize.y : 1);

    sf::Vector2f ppos = m_player.getPosition();
    sf::Vector2f pvel = m_player.getVelocity();

    std::vector<float> s(49, 0.f);

    // ── Header ────────────────────────────────────────────────────────────────
    s[0] = 0.0f;                                             // level_id
    s[1] = std::clamp(wf / REF_W, 0.f, 1.f);               // win_w_norm
    s[2] = std::clamp(hf / REF_H, 0.f, 1.f);               // win_h_norm
    s[3] = std::clamp(ppos.x / wf, 0.f, 1.f);              // player_x
    s[4] = std::clamp(ppos.y / hf, 0.f, 1.f);              // player_y
    s[5] = std::clamp(pvel.x / PLAYER_MAX_SPEED, -1.f, 1.f); // player_vx
    s[6] = std::clamp(pvel.y / PLAYER_MAX_SPEED, -1.f, 1.f); // player_vy
    s[7] = std::clamp(static_cast<float>(m_stepsSinceReward) / 300.f, 0.f, 1.f);
    s[8] = std::clamp(m_lastReward / 100.f, -1.f, 1.f);

    // ── Object slots — 8 nearest convex triangles (type 2) ───────────────────
    auto snaps = m_pool.getSnapshots(ppos, MAX_OBJ_SLOTS);
    for (int i = 0; i < MAX_OBJ_SLOTS; ++i) {
        int base = 9 + i * 5;
        const auto& snap = snaps[i];
        bool empty = (snap.center.x == 0.f && snap.center.y == 0.f &&
                      snap.velocity.x == 0.f && snap.velocity.y == 0.f);
        s[base + 0] = empty ? 0.f : 2.f / NUM_OBJ_TYPES;       // type id
        s[base + 1] = std::clamp(snap.center.x   / wf, 0.f, 1.f);
        s[base + 2] = std::clamp(snap.center.y   / hf, 0.f, 1.f);
        s[base + 3] = std::clamp(snap.velocity.x / MAX_VEL, -1.f, 1.f);
        s[base + 4] = std::clamp(snap.velocity.y / MAX_VEL, -1.f, 1.f);
    }
    return s;
}

std::vector<float> Level1::reset(sf::Vector2u windowSize) {
    resetScore();
    m_speed  = static_cast<float>(INITIAL_OBSTACLE_SPEED);
    m_player.reset(windowSize);
    m_pool = ConvexObstaclePool{};
    m_gameOver = false;
    return getState();
}

std::vector<float> Level1::step(int action, float& reward, bool& isDone) {
    auto winSize = m_window.getSize();
    float dt     = 1.f / 60.f;

    applyAction(m_player, action);
    m_player.update(dt);

    if (getScore() < SCORE_SPEED_THRESHOLD)
        m_speed = static_cast<float>(INITIAL_OBSTACLE_SPEED) *
                  std::pow(SCORE_SPEED_EXPONENT, getScore() / 100.f);

    m_pool.update(winSize, m_speed);

    reward = 0.1f;
    isDone = false;

    if (m_pool.collidesWithPlayer(m_player.getBounds())) {
        reward = -100.f;
        isDone = true;
    }

    addScore(1);

    // Track reward timing
    if (reward != 0.1f) m_stepsSinceReward = 0;
    else                ++m_stepsSinceReward;
    m_lastReward = reward;

    return getState();
}
