#pragma once

// ─── Minimum allowed window dimensions ──────────────────────────────────────
constexpr int MIN_WINDOW_WIDTH  = 640;
constexpr int MIN_WINDOW_HEIGHT = 480;

// ─── Default window dimensions ───────────────────────────────────────────────
constexpr int DEFAULT_WINDOW_WIDTH  = 900;
constexpr int DEFAULT_WINDOW_HEIGHT = 900;

// ─── Physics (velocity-based movement) ───────────────────────────────────────
constexpr float PLAYER_ACCEL     = 0.8f;   // velocity added per frame per key
constexpr float PLAYER_DRAG      = 0.92f;  // water resistance multiplier (< 1.0)
constexpr float PLAYER_MAX_SPEED = 9.0f;   // terminal velocity cap

// ─── Dash ability ─────────────────────────────────────────────────────────────
constexpr int   DASH_IFRAME_FRAMES   = 10;    // invincibility frame count
constexpr float DASH_VEL_MULTIPLIER  = 4.0f;  // burst velocity multiplier
constexpr float DASH_COOLDOWN_SEC    = 3.0f;
constexpr float DASH_SHAKE_INTENSITY = 3.0f;
constexpr int   DASH_SHAKE_FRAMES    = 6;

// ─── Graze mechanic ───────────────────────────────────────────────────────────
constexpr float GRAZE_INFLATE_PX = 22.f;  // added each side for graze hitbox
constexpr int   GRAZE_SCORE_PER_FRAME = 1; // score awarded per frame of graze

// ─── Sonar pulse ability ──────────────────────────────────────────────────────
constexpr float SONAR_EXPAND_SPEED = 220.f;  // radius growth px/sec
constexpr float SONAR_MAX_RADIUS   = 350.f;  // max radius before expiring
constexpr float SONAR_SLOW_FACTOR  = 0.30f;  // obstacle speed multiplier when slowed
constexpr float SONAR_COOLDOWN_SEC = 5.0f;

// ─── Screen shake ────────────────────────────────────────────────────────────
constexpr float SHAKE_INTENSITY_DEATH = 5.0f;
constexpr int   SHAKE_FRAMES_DEATH    = 10;

// ─── Hit stop ────────────────────────────────────────────────────────────────
constexpr int HIT_STOP_MS = 500;   // milliseconds freeze on death

// ─── Gameplay tuning ─────────────────────────────────────────────────────────
constexpr int   INITIAL_OBSTACLE_SPEED  = 4;
constexpr float OBSTACLE_SPAWN_CHANCE   = 50.f;
constexpr int   SCORE_SPEED_THRESHOLD   = 500;
constexpr float SCORE_SPEED_EXPONENT    = 1.3f;

// ─── Asset paths ─────────────────────────────────────────────────────────────
constexpr const char* ASSET_FONT         = "assets/Bangers.ttf";
constexpr const char* ASSET_MUSIC        = "assets/back.mp3";
constexpr const char* ASSET_SUBMARINE    = "assets/submarine.png";
constexpr const char* ASSET_CRAB         = "assets/crab.png";
constexpr const char* ASSET_FISH         = "assets/fish.png";
constexpr const char* ASSET_OCTOPUS      = "assets/oct.png";
constexpr const char* ASSET_TREASURE     = "assets/treasurechest.png";
constexpr const char* ASSET_URCHIN       = "assets/urchin.png";
