#pragma once
#include <SFML/Window/Keyboard.hpp>

// ─── InputHandler ─────────────────────────────────────────────────────────────
// Translates live keyboard state into the same discrete action enum used by
// the RL agent, so the engine (Player, Level) is never coupled to the UI.
//
//   action = baseDir + 5 * modifier
//
//   Base Direction:
//     0 = Up    1 = Down    2 = Left    3 = Right    4 = None
//   Modifier:
//     0 = Normal (0-4)
//     1 = Dash pressed (+5)
//     2 = Sonar pressed (+10)
//     (If both Dash & Sonar pressed, Sonar takes priority here for simplicity)
struct InputHandler {
    // Returns the composite 0-14 action index
    static int pollCompositeAction() noexcept {
        int base = 4; // None
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    base = 0;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  base = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  base = 2;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) base = 3;

        int mod = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))         mod = 2; // Sonar
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) mod = 1; // Dash

        return base + (mod * 5);
    }
    
    // For convenience in Level::update() to un-pack the composite action
    static void decodeAction(int composite, int& baseDir, bool& isDash, bool& isSonar) noexcept {
        baseDir = composite % 5;
        int mod = composite / 5;
        isDash  = (mod == 1);
        isSonar = (mod == 2);
    }
};
