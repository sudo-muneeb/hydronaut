#pragma once
#include <SFML/Graphics.hpp>

// ─── Main Menu ────────────────────────────────────────────────────────────────
// Draws a deep-sea blue gradient background (no image dependency).
// Returns the selected level (1, 2, 3) or -1 if the window is closed.
class Menu {
public:
    explicit Menu(sf::RenderWindow& window);

    // Returns: 1=Level1, 2=Level2, 3=Level3, -1=quit/closed
    int run();

private:
    void updateHighlight();
    void drawBackground();

    sf::RenderWindow& m_window;
    sf::Font&         m_font;   // Reference from AssetManager
    sf::Text          m_items[3];
    sf::RectangleShape m_highlight;
    int               m_selected = 0;
};
