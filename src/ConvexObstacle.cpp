#include "ConvexObstacle.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <limits>
#include <numeric>

static constexpr float OBS_SIZE_FACTOR = 0.02f;

ConvexObstacle::ConvexObstacle(sf::Vector2u windowSize)
    : m_speed(4.f), m_rotation(0.f)
{
    reset(windowSize);
}

void ConvexObstacle::reset(sf::Vector2u windowSize) {
    if (windowSize.x == 0 || windowSize.y == 0)
        throw std::invalid_argument("ConvexObstacle: window size must be non-zero");

    float size = std::min(windowSize.x, windowSize.y) * OBS_SIZE_FACTOR * 2.5f;

    m_shape.setPointCount(3);
    m_shape.setPoint(0, sf::Vector2f(0, 0));
    m_shape.setPoint(1, sf::Vector2f(size / 2.f, size));
    m_shape.setPoint(2, sf::Vector2f(size, 0));
    m_shape.setFillColor(sf::Color(255, 140, 0));
    m_shape.setOutlineColor(sf::Color(255, 200, 50));
    m_shape.setOutlineThickness(1.5f);

    m_x = static_cast<float>(windowSize.x);
    m_y = static_cast<float>(std::rand() % windowSize.y);
    m_shape.setPosition(m_x, m_y);
    m_lastVel = {0.f, 0.f};
}

void ConvexObstacle::setSpeed(float speed) noexcept { m_speed = speed; }

void ConvexObstacle::update(sf::Vector2u /*windowSize*/) noexcept {
    float step = m_speed * m_speedMult;
    m_x -= step;
    m_rotation += 2.0f * m_speedMult;
    m_shape.setPosition(m_x, m_y);
    m_shape.setRotation(m_rotation);
    // Triangles only move horizontally
    m_lastVel = {-step, 0.f};
}

void ConvexObstacle::draw(sf::RenderWindow& window) const noexcept {
    window.draw(m_shape);
}

sf::FloatRect ConvexObstacle::getBounds() const noexcept {
    return m_shape.getGlobalBounds();
}

// ─── Pool ─────────────────────────────────────────────────────────────────────
void ConvexObstaclePool::update(sf::Vector2u windowSize, float speed) {
    m_speed = speed;

    if (m_obstacles.capacity() < 30)
        m_obstacles.reserve(30);

    if (std::rand() % 50 == 0)
        spawnOne(windowSize);

    for (auto& obs : m_obstacles)
        obs.update(windowSize);

    m_obstacles.erase(
        std::remove_if(m_obstacles.begin(), m_obstacles.end(),
            [](const ConvexObstacle& o) {
                return o.getBounds().left + o.getBounds().width < 0;
            }),
        m_obstacles.end());
}

void ConvexObstaclePool::draw(sf::RenderWindow& window) const {
    for (const auto& obs : m_obstacles)
        obs.draw(window);
}

void ConvexObstaclePool::drawDebugBounds(sf::RenderWindow& window) const {
    for (const auto& obs : m_obstacles) {
        sf::FloatRect b = obs.getBounds();
        sf::RectangleShape r({b.width, b.height});
        r.setPosition({b.left, b.top});
        r.setFillColor(sf::Color::Transparent);
        r.setOutlineColor(sf::Color(255, 60, 60, 200));
        r.setOutlineThickness(2.f);
        window.draw(r);
    }
}

bool ConvexObstaclePool::collidesWithPlayer(sf::FloatRect playerBounds) const noexcept {
    for (const auto& obs : m_obstacles)
        if (playerBounds.intersects(obs.getBounds()))
            return true;
    return false;
}

std::vector<ObstacleSnapshot>
ConvexObstaclePool::getSnapshots(sf::Vector2f playerPos,
                                  std::size_t  maxCount) const noexcept {
    // Collect all active obstacles with their squared-distance to player
    struct Candidate {
        float        distSq;
        sf::Vector2f center;
        sf::Vector2f velocity;
    };
    std::vector<Candidate> cands;
    cands.reserve(m_obstacles.size());

    for (const auto& obs : m_obstacles) {
        sf::FloatRect b = obs.getBounds();
        sf::Vector2f  c{ b.left + b.width * 0.5f, b.top + b.height * 0.5f };
        float dx = c.x - playerPos.x;
        float dy = c.y - playerPos.y;
        cands.push_back({ dx*dx + dy*dy, c, obs.getVelocity() });
    }

    // Sort by proximity then take the nearest maxCount
    std::sort(cands.begin(), cands.end(),
              [](const Candidate& a, const Candidate& b) {
                  return a.distSq < b.distSq;
              });

    std::vector<ObstacleSnapshot> result;
    result.reserve(maxCount);
    for (std::size_t i = 0; i < maxCount && i < cands.size(); ++i)
        result.push_back({ cands[i].center, cands[i].velocity });

    // Pad with zero entries if fewer than maxCount
    while (result.size() < maxCount)
        result.push_back({ {0.f, 0.f}, {0.f, 0.f} });

    return result;
}

void ConvexObstaclePool::spawnOne(sf::Vector2u windowSize) {
    ConvexObstacle obs(windowSize);
    obs.setSpeed(m_speed);
    m_obstacles.emplace_back(std::move(obs));
}

// ─── Memento Pattern API ────────────────────────────────────────────────────
ConvexObstacle::Snapshot ConvexObstacle::saveState() const {
    return { m_x, m_y, m_speed, m_rotation };
}

void ConvexObstacle::restoreState(const Snapshot& snap, sf::Vector2u windowSize) {
    m_x        = snap.x;
    m_y        = snap.y;
    m_speed    = snap.speed;
    m_rotation = snap.rotation;
    m_shape.setPosition(m_x, m_y);
    m_shape.setRotation(m_rotation);
}

ConvexObstaclePool::Snapshot ConvexObstaclePool::saveState() const {
    Snapshot snap;
    snap.speed = m_speed;
    snap.obstacles.reserve(m_obstacles.size());
    for (const auto& obs : m_obstacles) {
        snap.obstacles.push_back(obs.saveState());
    }
    return snap;
}

void ConvexObstaclePool::restoreState(const Snapshot& snap, sf::Vector2u windowSize) {
    m_speed = snap.speed;
    m_obstacles.clear();
    m_obstacles.reserve(snap.obstacles.size());
    for (const auto& oSnap : snap.obstacles) {
        ConvexObstacle obs(windowSize);
        obs.restoreState(oSnap, windowSize);
        m_obstacles.push_back(std::move(obs));
    }
}
