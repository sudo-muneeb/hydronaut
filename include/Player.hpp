#pragma once
#include <SFML/Graphics.hpp>

// ─── Player ───────────────────────────────────────────────────────────────────
// Handles the submarine sprite, keyboard input, and window-relative clamping.
class Player {
public:
    explicit Player(sf::Vector2u windowSize);

    // Process keyboard state and move; clamps to window bounds.
    void handleInput(sf::Vector2u windowSize);

    void draw(sf::RenderWindow& window) const;

    sf::FloatRect getBounds() const;
    sf::Vector2f  getPosition() const;

    // Reset to starting position (left-centre of window).
    void reset(sf::Vector2u windowSize);

private:
    sf::Sprite m_sprite;
};
