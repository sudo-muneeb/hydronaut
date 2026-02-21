#include "Level2.hpp"

Level2::Level2(sf::RenderWindow& window)
    : Level(window)
    , m_player(window.getSize())
    , m_sine(window.getSize(), "urchin")
    , m_para(window.getSize(), "crab")
    , m_treasure(window.getSize())
{
}

bool Level2::update() {
    auto winSize = m_window.getSize();

    m_player.handleInput(winSize);
    m_sine.update(winSize);
    m_para.update(winSize);

    // Collect treasure
    if (m_player.getBounds().intersects(m_treasure.getBounds())) {
        m_treasure.respawn(winSize);
        addScore(10);
    }

    // Collision with enemies
    if (m_player.getBounds().intersects(m_sine.getBounds()) ||
        m_player.getBounds().intersects(m_para.getBounds()))
        return true;

    return false;
}

void Level2::draw() {
    m_treasure.draw(m_window);
    m_sine.draw(m_window);
    m_para.draw(m_window);
    m_player.draw(m_window);
}
