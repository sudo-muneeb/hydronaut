#include "Level1.hpp"
#include "Constants.hpp"
#include <cmath>

Level1::Level1(sf::RenderWindow& window)
    : Level(window), m_player(window.getSize())
{
}

bool Level1::update() {
    auto winSize = m_window.getSize();

    m_player.handleInput(winSize);

    // Increase speed with score
    if (getScore() < SCORE_SPEED_THRESHOLD)
        m_speed = INITIAL_OBSTACLE_SPEED *
                  std::pow(SCORE_SPEED_EXPONENT, getScore() / 100.f);

    m_pool.update(winSize, m_speed);

    addScore(1);

    if (m_pool.collidesWithPlayer(m_player.getBounds()))
        return true; // game over

    return false;
}

void Level1::draw() {
    m_pool.draw(m_window);
    m_player.draw(m_window);
}
