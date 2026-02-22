#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Player;  // forward-decl for applyAction helper

// ─── Abstract Level base ──────────────────────────────────────────────────────
// Provides the shared game loop, HUD, blue gradient background, pause/resume,
// screen shake, sonar pulse (X key), graze HUD, hit-stop, and debug overlay.
//
// For RL training, set isTrainingMode = true via setTrainingMode() BEFORE
// calling reset() / step().  The normal run() loop is never called in that case.
//
// Derived classes implement:
//   bool update()  — move obstacles, check collision, return true = player dead
//   void draw()    — draw enemies, player, treasure
//   std::vector<float> getState()  — 12-element normalised observation
//   std::vector<float> reset(sf::Vector2u)  — reset episode, return first obs
//   std::vector<float> step(int action, float& reward, bool& done)
class Level {
public:
    explicit Level(sf::RenderWindow& window);
    virtual ~Level() = default;

    // ─── Normal gameplay loop ─────────────────────────────────────────────
    // Runs until game-over or Escape. Returns final score.
    int run();

    // ─── RL training API ──────────────────────────────────────────────────
    void setTrainingMode(bool val) noexcept { m_trainingMode = val; }
    bool isTrainingMode()          const noexcept { return m_trainingMode; }

    virtual std::vector<float> getState()                                   const = 0;
    virtual std::vector<float> reset(sf::Vector2u windowSize)                     = 0;
    virtual std::vector<float> step(int action, float& reward, bool& isDone)      = 0;

    // Convenience: window size used by headless training loop
    sf::Vector2u getWindowSize() const noexcept { return m_window.getSize(); }

    // ─── Inference rendering ──────────────────────────────────────────────
    // Render one visual frame: background → shake → level sprites → HUD.
    // Call this from play.cpp after env->step() to show the full game view.
    void renderFrame();

    // ─── Virtual size (multi-screen training / live resize) ─────────────────
    // Override the physics size used by step() without resizing the OS window.
    // Training uses this to randomise screen sizes each episode.
    // Play mode uses it to recalibrate on window-resize events.
    void         setVirtualSize(sf::Vector2u sz) noexcept;
    sf::Vector2u getSimSize()                    const noexcept;

protected:
    // ─── Required overrides ────────────────────────────────────────────────
    virtual bool update() = 0;    // returns true → player dies → hit-stop
    virtual void draw()   = 0;    // draw level-specific objects

    // ─── RL helper ────────────────────────────────────────────────────────
    // Translate discrete action (0=Up,1=Down,2=Left,3=Right) into a velocity
    // impulse applied directly to the Player (bypasses keyboard polling).
    void applyAction(Player& player, int action) noexcept;

    // ─── Shared helpers for subclasses ────────────────────────────────────
    void drawBackground();
    void drawHUD();
    void addScore(int amount = 1) noexcept;
    int  getScore()               const noexcept { return m_score; }
    void resetScore()             noexcept {
        m_score = 0; m_grazeAcc = 0;
        m_stepsSinceReward = 0; m_lastReward = 0.f;
    }

    // Screen shake — trigger this when the player dashes or hits something.
    void triggerShake(int frames, float intensity) noexcept;

    // Sonar — returns the current obstacle speed multiplier (1.0 or SONAR_SLOW_FACTOR).
    float getSonarFactor() const noexcept;

    // Sonar readiness — returns true when the sonar ability can be fired.
    // Derived classes use this to populate the state vector at s[48].
    bool isSonarReady() const noexcept;

    // Graze — call from update(). Returns 1 if graze, 0 otherwise.
    // Does NOT fire if playerCore intersects obstacleBox (that's a lethal hit).
    int checkGraze(sf::FloatRect playerGraze,
                   sf::FloatRect playerCore,
                   sf::FloatRect obstacleBox) const noexcept;

    // Debug mode flag (toggled by F3 key).
    bool m_debugMode    = false;
    bool m_trainingMode = false;

    // RL reward-timing state (updated by each level's step())
    int   m_stepsSinceReward = 0;   // frames elapsed since last non-zero reward
    float m_lastReward       = 0.f; // reward value from the most recent step

    sf::RenderWindow& m_window;
    bool              m_paused      = false;
    bool              m_gameOver    = false;
    sf::Vector2u      m_virtualSize = {};  // 0 = use m_window.getSize()

private:
    void handleEvents();
    void applyShake();
    void restoreView();
    void drawSonarRing();
    void drawGrazeHUD();
    void drawDebugOverlay();
    void drawMetricsOverlay();
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
    // ─── Metrics ──────────────────────────────────────────────────────────
    bool      m_showMetrics = false;
    bool      m_f4KeyDown   = false;
    sf::Clock m_fpsMeasurementClock;
    int       m_frameMeasurementCount = 0;
    float     m_currentFps   = 0.f;
    float     m_logicTimeMs  = 0.f;
};
