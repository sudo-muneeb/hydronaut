#include "Level3.hpp"

Level3::Level3(sf::RenderWindow& window)
    : Level(window)
    , m_player(window.getSize())
    , m_sec(window.getSize(), "octopus")
    , m_expSine(window.getSize(), "fish")
    , m_treasure(window.getSize())
{
}

bool Level3::update() {
    auto winSize = m_window.getSize();

    m_player.handleInput(winSize);
    m_sec.update(winSize);
    m_expSine.update(winSize);

    // Collect treasure
    if (m_player.getBounds().intersects(m_treasure.getBounds())) {
        m_treasure.respawn(winSize);
        addScore(10);
    }

    // Collision with enemies
    if (m_player.getBounds().intersects(m_sec.getBounds()) ||
        m_player.getBounds().intersects(m_expSine.getBounds()))
        return true;

    return false;
}

void Level3::draw() {
    m_treasure.draw(m_window);
    m_sec.draw(m_window);
    m_expSine.draw(m_window);
    m_player.draw(m_window);
}
