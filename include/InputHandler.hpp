#pragma once
#include <SFML/Window/Keyboard.hpp>

// ─── InputHandler ─────────────────────────────────────────────────────────────
// Translates live keyboard state into the same discrete action enum used by
// the RL agent, so the engine (Player, Level) is never coupled to the UI.
//
// Action enum (matches Level::applyAction):
//   0 = Up    1 = Down    2 = Left    3 = Right    4 = None
//
// Usage (in Level::update() only — never called from RL step() path):
//   int  action = InputHandler::pollAction();
//   bool dash   = InputHandler::isDashPressed();
struct InputHandler {
    // Returns the highest-priority directional action pressed this frame.
    // Returns 4 (None) if no arrow key is held.
    static int pollAction() noexcept {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    return 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  return 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  return 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) return 3;
        return 4;  // no input
    }

    // True while Space is held — used to trigger Hydro-Dash.
    static bool isDashPressed() noexcept {
        return sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    }
};
