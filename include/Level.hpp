#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ─── Abstract Level base ──────────────────────────────────────────────────────
// Subclasses implement their own update/draw/isOver logic.
// The base provides: a shared game loop, score HUD, pause, game-over screen,
// and a blue gradient background drawn from the window size.
class Level {
public:
    explicit Level(sf::RenderWindow& window);
    virtual ~Level() = default;

    // Runs the full game loop until game-over; returns final score.
    int run();

protected:
    // Called each frame — move obstacles, detect collision, etc.
    // Returns true if the level is over (player died / time up).
    virtual bool update() = 0;

    // Draw level-specific objects (obstacles, treasure, …).
    virtual void draw() = 0;

    // Draw the deep-blue gradient background.
    void drawBackground();

    // Draw score HUD text.
    void drawHUD();

    // Draw game-over overlay, wait, then return.
    void showGameOver();

    // Increment internal score counter.
    void addScore(int amount = 1);

    int getScore() const { return m_score; }

    sf::RenderWindow& m_window;
    bool              m_paused     = false;
    bool              m_gameOver   = false;

private:
    void handlePauseKey(sf::Event& event);

    sf::Text    m_scoreText;
    sf::Text    m_pauseText;
    int         m_score      = 0;
    bool        m_pKeyDown   = false;
};
