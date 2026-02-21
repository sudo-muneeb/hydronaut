#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ─── Abstract Level base ──────────────────────────────────────────────────────
// Provides the shared game loop, HUD, blue gradient background, pause/resume,
// screen shake, sonar pulse (X key), graze HUD, hit-stop, and debug overlay.
//
// Derived classes implement:
//   bool update()  — move obstacles, check collision, return true = player dead
//   void draw()    — draw enemies, player, treasure
class Level {
public:
    explicit Level(sf::RenderWindow& window);
    virtual ~Level() = default;

    // Runs until game-over or Escape. Returns final score.
    int run();

protected:
    // ─── Required overrides ────────────────────────────────────────────────
    virtual bool update() = 0;    // returns true → player dies → hit-stop
    virtual void draw()   = 0;    // draw level-specific objects

    // ─── Shared helpers for subclasses ────────────────────────────────────
    void drawBackground();
    void drawHUD();
    void addScore(int amount = 1) noexcept;
    int  getScore()               const noexcept { return m_score; }

    // Screen shake — trigger this when the player dashes or hits something.
    void triggerShake(int frames, float intensity) noexcept;

    // Sonar — returns the current obstacle speed multiplier (1.0 or SONAR_SLOW_FACTOR).
    float getSonarFactor() const noexcept;

    // Graze — call from update(). Returns 1 if graze, 0 otherwise.
    // Does NOT fire if playerCore intersects obstacleBox (that's a lethal hit).
    int checkGraze(sf::FloatRect playerGraze,
                   sf::FloatRect playerCore,
                   sf::FloatRect obstacleBox) const noexcept;

    // Debug mode flag (toggled by F3 key).
    bool m_debugMode = false;

    sf::RenderWindow& m_window;
    bool              m_paused   = false;
    bool              m_gameOver = false;

private:
    void handleEvents();
    void applyShake();
    void restoreView();
    void drawSonarRing();
    void drawGrazeHUD();
    void drawDebugOverlay();
    void showGameOver();
    void showPauseOverlay();

    // ─── Score / HUD ──────────────────────────────────────────────────────
    int      m_score    = 0;
    int      m_grazeAcc = 0;   // graze points accumulated this run
    sf::Text m_scoreText;
    sf::Text m_pauseText;
    sf::Text m_grazeText;

    // ─── Pause ────────────────────────────────────────────────────────────
    bool m_pKeyDown  = false;

    // ─── Screen shake ─────────────────────────────────────────────────────
    int   m_shakeFrames    = 0;
    float m_shakeIntensity = 0.f;
    sf::View m_baseView;

    // ─── Sonar pulse ──────────────────────────────────────────────────────
    bool         m_sonarActive   = false;
    float        m_sonarRadius   = 0.f;
    sf::Vector2f m_sonarCenter;
    sf::Clock    m_sonarClock;
    bool         m_sonarFired    = false;
    sf::Clock    m_sonarCooldownClock;
    bool         m_xKeyDown      = false;
};
