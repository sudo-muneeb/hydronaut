#include "Player.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <algorithm>

// Scale the submarine to roughly 7% of window height
static constexpr float PLAYER_SCALE_FACTOR = 0.07f;

Player::Player(sf::Vector2u windowSize) {
    m_sprite.setTexture(AssetManager::instance().texture("submarine"));
    reset(windowSize);
}

void Player::reset(sf::Vector2u windowSize) {
    auto& tex = AssetManager::instance().texture("submarine");
    float scale = (windowSize.y * PLAYER_SCALE_FACTOR) /
                  static_cast<float>(tex.getSize().y);
    m_sprite.setScale(scale, scale);
    m_sprite.setPosition(windowSize.x * 0.05f,
                         windowSize.y * 0.5f);
}

void Player::handleInput(sf::Vector2u windowSize) {
    float speed = static_cast<float>(PLAYER_SPEED);
    auto  pos   = m_sprite.getPosition();
    auto  sz    = m_sprite.getGlobalBounds();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && pos.y > 0)
        m_sprite.move(0, -speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) &&
        pos.y + sz.height < windowSize.y)
        m_sprite.move(0, speed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
        pos.x + sz.width < windowSize.x * 0.9f)
        m_sprite.move(speed, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && pos.x > 0)
        m_sprite.move(-speed, 0);
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(m_sprite);
}

sf::FloatRect Player::getBounds() const {
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const {
    return m_sprite.getPosition();
}
