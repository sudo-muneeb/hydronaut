#pragma once

// ─── Minimum allowed window dimensions ──────────────────────────────────────
constexpr int MIN_WINDOW_WIDTH  = 640;
constexpr int MIN_WINDOW_HEIGHT = 480;

// ─── Default window dimensions (overridable at runtime) ─────────────────────
constexpr int DEFAULT_WINDOW_WIDTH  = 900;
constexpr int DEFAULT_WINDOW_HEIGHT = 900;

// ─── Gameplay tuning ─────────────────────────────────────────────────────────
constexpr int   PLAYER_SPEED            = 6;
constexpr int   INITIAL_OBSTACLE_SPEED  = 4;
constexpr float OBSTACLE_SPAWN_CHANCE   = 50.f;  // 1-in-N chance per frame
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
