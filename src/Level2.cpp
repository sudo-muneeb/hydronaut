#include "Level2.hpp"
#include "Constants.hpp"
#include "SpriteBounds.hpp"
#include "AssetManager.hpp"
#include <algorithm>
#include <cmath>

static constexpr int   MAX_OBJ_SLOTS = 8;
static constexpr int   NUM_OBJ_TYPES = 6;
static constexpr float MAX_VEL       = 10.f;
static constexpr float REF_W         = 1920.f;
static constexpr float REF_H         = 1080.f;

Level2::Level2(sf::RenderWindow& window)
    : Level(window)
    , m_player(window.getSize())
    , m_sine(window.getSize(), "urchin")
    , m_para(window.getSize(), "crab")
    , m_treasure(window.getSize())
{
}

// ─── Normal gameplay update ───────────────────────────────────────────────────
bool Level2::update() {
    auto  winSize = m_window.getSize();
    float dt      = 1.f / 60.f;
    float sonar   = getSonarFactor();

    bool dashed = m_player.handleInput(winSize);
    m_player.update(dt);
    if (dashed) triggerShake(DASH_SHAKE_FRAMES, DASH_SHAKE_INTENSITY);

    m_sine.setSpeedMultiplier(sonar);
    m_para.setSpeedMultiplier(sonar);
    m_sine.update(winSize);
    m_para.update(winSize);

    if (m_player.getBounds().intersects(m_treasure.getBounds())) {
        m_treasure.respawn(winSize);
        addScore(10);
    }

    int gs = checkGraze(m_player.getGrazeBounds(), m_player.getBounds(),
                        m_sine.getBounds());
    gs    += checkGraze(m_player.getGrazeBounds(), m_player.getBounds(),
                        m_para.getBounds());
    if (gs > 0) addScore(GRAZE_SCORE_PER_FRAME * gs);

    if (!m_player.isDashing()) {
        auto& am = AssetManager::instance();
        if (pixelPerfectOverlap(m_player.getSprite(), am.image("submarine"),
                                m_sine.getSprite(),   am.image("urchin"))   ||
            pixelPerfectOverlap(m_player.getSprite(), am.image("submarine"),
                                m_para.getSprite(),   am.image("crab")))    {
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
        auto& am = AssetManager::instance();
        // Draw PIXEL-ACCURATE collision hulls (green = player, red = obstacle)
        m_player.drawDebugHitboxes(m_window);   // inner lethal rect
        drawDebugHull(m_window, m_player.getSprite(), am.hullUV("submarine"),
                      sf::Color(80, 255, 80, 220));
        drawDebugHull(m_window, m_sine.getSprite(),   am.hullUV("urchin"),
                      sf::Color(255, 80, 80, 220));
        drawDebugHull(m_window, m_para.getSprite(),   am.hullUV("crab"),
                      sf::Color(255, 80, 80, 220));
    }
}

// ─── RL Environment API ───────────────────────────────────────────────────────
static auto centreOf = [](sf::FloatRect b) -> sf::Vector2f {
    return { b.left + b.width * 0.5f, b.top + b.height * 0.5f };
};

std::vector<float> Level2::getState() const {
    auto winSize = m_window.getSize();
    float wf = static_cast<float>(winSize.x ? winSize.x : 1);
    float hf = static_cast<float>(winSize.y ? winSize.y : 1);

    sf::Vector2f ppos  = m_player.getPosition();
    sf::Vector2f pvel  = m_player.getVelocity();
    sf::Vector2f sC    = centreOf(m_sine.getBounds());
    sf::Vector2f pC    = centreOf(m_para.getBounds());
    sf::Vector2f tC    = centreOf(m_treasure.getBounds());
    sf::Vector2f sV    = m_sine.getVelocity();
    sf::Vector2f pV    = m_para.getVelocity();

    std::vector<float> s(49, 0.f);

    s[0] = 0.5f;
    s[1] = std::clamp(wf / REF_W, 0.f, 1.f);
    s[2] = std::clamp(hf / REF_H, 0.f, 1.f);
    s[3] = std::clamp(ppos.x / wf, 0.f, 1.f);
    s[4] = std::clamp(ppos.y / hf, 0.f, 1.f);
    s[5] = std::clamp(pvel.x / PLAYER_MAX_SPEED, -1.f, 1.f);
    s[6] = std::clamp(pvel.y / PLAYER_MAX_SPEED, -1.f, 1.f);
    s[7] = std::clamp(static_cast<float>(m_stepsSinceReward) / 300.f, 0.f, 1.f);
    s[8] = std::clamp(m_lastReward / 100.f, -1.f, 1.f);

    s[9]  = 3.f / NUM_OBJ_TYPES;
    s[10] = std::clamp(sC.x / wf, 0.f, 1.f);
    s[11] = std::clamp(sC.y / hf, 0.f, 1.f);
    s[12] = std::clamp(sV.x / MAX_VEL, -1.f, 1.f);
    s[13] = std::clamp(sV.y / MAX_VEL, -1.f, 1.f);

    s[14] = 4.f / NUM_OBJ_TYPES;
    s[15] = std::clamp(pC.x / wf, 0.f, 1.f);
    s[16] = std::clamp(pC.y / hf, 0.f, 1.f);
    s[17] = std::clamp(pV.x / MAX_VEL, -1.f, 1.f);
    s[18] = std::clamp(pV.y / MAX_VEL, -1.f, 1.f);

    s[19] = 1.f / NUM_OBJ_TYPES;
    s[20] = std::clamp(tC.x / wf, 0.f, 1.f);
    s[21] = std::clamp(tC.y / hf, 0.f, 1.f);

    return s;
}

std::vector<float> Level2::reset(sf::Vector2u windowSize) {
    resetScore();
    m_player.reset(windowSize);
    m_sine.reset(windowSize);
    m_para.reset(windowSize);
    m_treasure.respawn(windowSize);
    m_gameOver = false;
    return getState();
}

std::vector<float> Level2::step(int action, float& reward, bool& isDone) {
    auto  winSize = getSimSize();           // virtual size for multi-screen training
    float dt      = 1.f / 60.f;

    applyAction(m_player, action);
    m_player.setWindowSize(winSize);
    m_player.update(dt);
    m_sine.update(winSize);
    m_para.update(winSize);

    isDone = false;

    // ── Distance-to-treasure shaping ──────────────────────────────────────────
    sf::Vector2f pC = centreOf(m_player.getBounds());
    sf::Vector2f tC = centreOf(m_treasure.getBounds());
    float dx = pC.x - tC.x,  dy = pC.y - tC.y;
    float curDist = std::sqrt(dx*dx + dy*dy);

    float approach = 0.f;
    if (m_prevDistToTreasure > 0.f)
        approach = (m_prevDistToTreasure - curDist) * 0.10f;  // +ve = closer
    m_prevDistToTreasure = curDist;

    // Base: approach bonus - step penalty (forces active treasure chasing)
    reward = approach - 0.05f;

    // ── Treasure collected ────────────────────────────────────────────────
    if (m_player.getBounds().intersects(m_treasure.getBounds())) {
        m_treasure.respawn(winSize);
        addScore(10);
        reward += 100.f;
        m_prevDistToTreasure = -1.f;  // reset shaping for new treasure position
    }

    // ── Lethal collision ───────────────────────────────────────────────────
    auto& am = AssetManager::instance();
    if (pixelPerfectOverlap(m_player.getSprite(), am.image("submarine"),
                            m_sine.getSprite(),   am.image("urchin"))   ||
        pixelPerfectOverlap(m_player.getSprite(), am.image("submarine"),
                            m_para.getSprite(),   am.image("crab")))    {
        reward = -100.f;
        isDone = true;
    }

    m_stepsSinceReward = (reward > 0.f) ? 0 : m_stepsSinceReward + 1;
    m_lastReward = reward;

    return getState();
}
