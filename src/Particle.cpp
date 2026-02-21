#include "Particle.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// ─── lifetime of one bubble in seconds ───────────────────────────────────────
static constexpr float BUBBLE_LIFE = 0.55f;

void ParticleEmitter::emit(sf::Vector2f origin, sf::Vector2f baseVel, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.pos    = origin;
        // Drift leftward, small random vertical spread
        float randY = (static_cast<float>(std::rand() % 100) - 50.f) * 0.03f;
        p.vel    = sf::Vector2f(baseVel.x * 0.4f - 1.0f, baseVel.y * 0.3f + randY);
        p.alphaf = 190.f;
        p.life   = BUBBLE_LIFE * (0.6f + 0.4f * (std::rand() % 100) / 100.f);
        p.radius = 2.f + (std::rand() % 4);
        m_particles.push_back(p);
    }
}

void ParticleEmitter::update(float dt) noexcept {
    for (auto& p : m_particles) {
        p.pos   += p.vel * dt * 60.f;   // scale by 60 so dt=1/60 gives original speed
        p.vel.x -= 0.05f;               // additional leftward drift
        p.alphaf -= (255.f / BUBBLE_LIFE) * dt;
        p.life   -= dt;
    }
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p) { return p.life <= 0.f || p.alphaf <= 0.f; }),
        m_particles.end());
}

void ParticleEmitter::draw(sf::RenderWindow& window) const {
    for (const auto& p : m_particles) {
        sf::CircleShape circle(p.radius);
        circle.setOrigin(p.radius, p.radius);
        circle.setPosition(p.pos);
        sf::Uint8 a = static_cast<sf::Uint8>(std::max(0.f, p.alphaf));
        circle.setFillColor(sf::Color(140, 210, 255, a));
        window.draw(circle);
    }
}
