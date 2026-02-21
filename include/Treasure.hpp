#pragma once
#include <SFML/Graphics.hpp>

// ─── Treasure chest ───────────────────────────────────────────────────────────
// Spawns at a random position (within 15‒85 % of window to stay visible).
// Respawns when collected by the player.
class Treasure {
public:
    explicit Treasure(sf::Vector2u windowSize);

    // Teleport to a new random position inside the window.
    void respawn(sf::Vector2u windowSize);

    void draw(sf::RenderWindow& window) const;

    sf::FloatRect getBounds() const;

private:
    sf::Sprite m_sprite;
};
