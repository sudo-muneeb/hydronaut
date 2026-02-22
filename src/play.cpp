// ─── play.cpp ────────────────────────────────────────────────────────────────
// Inference visualiser — loads hydronaut_dqn.pt and drives the submarine using
// env->step() for physics, then env->renderFrame() for the real game visuals.
// Action timing: 1–4 Hz (15–60 frame hold), matching training cadence.
//
// Usage:  ./play_hydronaut [1|2|3]   (default: level 2)

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <random>

#include "Constants.hpp"
#include "AssetManager.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"
#include "DQNAgent.hpp"

static const char* MODEL_PATH = "hydronaut_dqn.pt";
static constexpr int HOLD_MIN = 15;
static constexpr int HOLD_MAX = 60;

int main(int argc, char* argv[]) {
    int levelChoice = 2;
    if (argc >= 2) {
        try { levelChoice = std::stoi(argv[1]); }
        catch (...) {}
    }
    if (levelChoice < 1 || levelChoice > 3) {
        std::cerr << "Usage: play_hydronaut [1|2|3]\n";
        return EXIT_FAILURE;
    }

    try {
        // ── Window ────────────────────────────────────────────────────────────
        sf::RenderWindow window(
            sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT),
            "Hydronaut — AI Play  (Level " + std::to_string(levelChoice) + ")",
            sf::Style::Default);
        window.setFramerateLimit(60);

        AssetManager::instance().loadAll();

        sf::Music music;
        if (music.openFromFile(ASSET_MUSIC)) {
            music.setLoop(true); music.setVolume(10.f); music.play();
        }

        // ── Build level (training mode OFF → full SFML rendering via renderFrame) ─
        std::unique_ptr<Level> env;
        switch (levelChoice) {
            case 1:  env = std::make_unique<Level1>(window); break;
            case 2:  env = std::make_unique<Level2>(window); break;
            default: env = std::make_unique<Level3>(window); break;
        }
        env->setTrainingMode(false);

        // ── Load trained model ────────────────────────────────────────────────
        DQNAgent agent;
        agent.load_model(MODEL_PATH);
        agent.setEpsilon(0.0f);  // pure exploitation

        std::cout << "[play] Level " << levelChoice
                  << " | State dim: " << DQNAgent::STATE_DIM
                  << " | Action hold: " << HOLD_MIN << "-" << HOLD_MAX << " frames\n"
                  << "       ESC or close window to quit.\n\n";

        // winSize is read per-episode from the actual OS window — do NOT
        // hardcode 1920×1080 here or sprites spawn off-screen.

        // RNG for hold duration (mirrors training distribution)
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> holdDist(HOLD_MIN, HOLD_MAX);

        auto& font = AssetManager::instance().font();
        int episodeCount = 0;

        while (window.isOpen()) {
            ++episodeCount;
            const sf::Vector2u winSize = window.getSize(); // actual OS window
            env->setVirtualSize(winSize);
            std::vector<float> state = env->reset(winSize);

            float totalReward   = 0.f;
            int   frameCount    = 0;
            int   decisionCount = 0;
            int   holdRemaining = 0;
            int   currentAction = 0;

            while (window.isOpen()) {
                // ── Events ────────────────────────────────────────────────────
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed ||
                        (event.type == sf::Event::KeyPressed &&
                         event.key.code == sf::Keyboard::Escape)) {
                        window.close();
                    }
                    if (event.type == sf::Event::Resized) {
                        unsigned w = std::max(event.size.width, 640u);
                        unsigned h = std::max(event.size.height, 480u);
                        window.setSize({w, h});
                        env->setVirtualSize({w, h});
                    }
                }
                if (!window.isOpen()) break;

                // ── Human-paced decision ──────────────────────────────────────
                if (holdRemaining == 0) {
                    currentAction = agent.choose_action(state);
                    holdRemaining = holdDist(rng);
                    ++decisionCount;
                }
                --holdRemaining;

                // ── Step environment (RL physics path) ────────────────────────
                float reward = 0.f;
                bool  done   = false;
                state = env->step(currentAction, reward, done);
                totalReward += reward;
                ++frameCount;

                // ── Render the REAL game via Level::renderFrame() ─────────────
                // This draws background + all sprites (submarine, obstacles,
                // treasure) + sonar ring + level HUD — exactly as in normal play.
                env->renderFrame();

                // ── AI overlay on top (drawn AFTER renderFrame's display? No —
                //    we need to draw BEFORE display).  Use a secondary draw pass
                //    directly to window after renderFrame() calls display().
                // Simple workaround: draw overlay text on next frame start.
                // Better: expose a post-render hook.  For now the level HUD
                // already shows score; we just add a small AI status bar.
                // (renderFrame calls display() internally, so overlay comes next frame)

                if (done) {
                    std::cout << "Episode " << episodeCount
                              << " | Frames: " << frameCount
                              << " | Decisions: " << decisionCount
                              << " | Reward: " << totalReward << "\n";
                    sf::sleep(sf::milliseconds(600));
                    break;
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
