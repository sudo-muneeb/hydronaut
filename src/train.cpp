// ─── train.cpp ───────────────────────────────────────────────────────────────
// Headless training loop for the Hydronaut DQN agent.
// State vector: 49 elements (9 header + 8×5 generic object slots)
// Action timing: human-paced — each action held for 15–60 frames (~1–4 Hz)
//
// Usage:
//   ./train_hydronaut
//   ./train_hydronaut 1000   # custom episode count

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <iomanip>
#include <random>

#include "Constants.hpp"
#include "AssetManager.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"
#include "ReplayBuffer.hpp"
#include "DQNAgent.hpp"

// ─── Training configuration ───────────────────────────────────────────────────
static constexpr int   EPISODES      = 2000;
static constexpr int   MAX_STEPS     = 8000;  // more time per episode for treasure-seeking
static constexpr int   LEARN_EVERY   = 4;
static constexpr int   WARMUP_EXP    = 64;
static constexpr int   HOLD_MIN      = 20;    // tighter range reduces variance
static constexpr int   HOLD_MAX      = 50;
static constexpr int   PRINT_EVERY   = 10;
static const char*     MODEL_PATH    = "hydronaut_dqn.pt";

int main(int argc, char* argv[]) {
    int episodes = EPISODES;
    if (argc >= 2) {
        try { episodes = std::stoi(argv[1]); }
        catch (...) { std::cerr << "Invalid episode count; using " << EPISODES << "\n"; }
    }

    try {
        // Headless window: correct game dimensions so obstacle constructors
        // initialise without triggering divide-by-tiny in trig functions.
        // setVisible(false) keeps it off-screen.
        sf::RenderWindow window(
            sf::VideoMode(1920, 1080),
            "Training", sf::Style::None);
        window.setVisible(false);

        AssetManager::instance().loadAll();
        std::cout << "[train] Assets loaded.\n";
        std::cout << "[train] State dim : " << DQNAgent::STATE_DIM << "\n";

        // ── Levels ────────────────────────────────────────────────────────────
        Level1 lvl1(window); lvl1.setTrainingMode(true);
        Level2 lvl2(window); lvl2.setTrainingMode(true);
        Level3 lvl3(window); lvl3.setTrainingMode(true);

        std::array<Level*, 3> levels{ &lvl1, &lvl2, &lvl3 };

        // ── RL components ─────────────────────────────────────────────────────
        ReplayBuffer buffer(50'000);
        DQNAgent     agent;

        std::cout << "[train] Action hold: " << HOLD_MIN << "-" << HOLD_MAX
                  << " frames (~" << (60/HOLD_MAX) << "-" << (60/HOLD_MIN)
                  << " Hz, human-paced)\n";
        std::cout << "[train] Multi-screen: 640x480 to 1920x1080 randomisation\n\n";

        // Initial reset with a fixed large size; each episode randomises.
        sf::Vector2u trainSize(1920u, 1080u);
        lvl1.reset(trainSize);
        lvl2.reset(trainSize);
        lvl3.reset(trainSize);

        // RNG for action hold duration and screen sizes
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> holdDist(HOLD_MIN, HOLD_MAX);
        std::uniform_int_distribution<unsigned> widthDist(640, 1920);
        std::uniform_int_distribution<unsigned> heightDist(480, 1080);

        std::cout << "[train] Starting " << episodes << " episodes "
                  << "(round-robin: L1 → L2 → L3)\n\n";

        int globalFrame = 0;

        for (int ep = 0; ep < episodes; ++ep) {
            Level* env = levels[ep % 3];

            // Drain any pending SFML events from the hidden window
            { sf::Event e; while (window.pollEvent(e)) {} }

            // Randomise virtual screen size for scale-invariant learning
            sf::Vector2u randSize(widthDist(rng), heightDist(rng));
            env->setVirtualSize(randSize);

            std::vector<float> state = env->reset(randSize);

            float episodeReward   = 0.f;
            int   decisionCount   = 0;

            // ── Human-paced action loop ────────────────────────────────────────
            int   holdRemaining   = 0;
            int   currentAction   = 0;
            float accReward       = 0.f;
            std::vector<float> decisionState;

            for (int frame = 0; frame < MAX_STEPS; ++frame) {

                // Time to make a new decision?
                if (holdRemaining == 0) {
                    // Store previous experience (skip on very first decision)
                    if (decisionCount > 0 && buffer.size() > 0) {
                        // already pushed inside the hold-end branch below
                    }
                    decisionState  = state;
                    currentAction  = agent.choose_action(state);
                    holdRemaining  = holdDist(rng);
                    accReward      = 0.f;
                    ++decisionCount;
                }

                // Execute one physics frame
                float stepReward = 0.f;
                bool  done       = false;
                std::vector<float> nextState = env->step(currentAction, stepReward, done);

                accReward     += stepReward;
                episodeReward += stepReward;
                --holdRemaining;
                ++globalFrame;

                // Push experience at the END of each hold (or on done)
                bool holdEnded = (holdRemaining == 0);
                if (holdEnded || done) {
                    buffer.push({ decisionState, currentAction,
                                  accReward, nextState, done });
                    decisionState = nextState;   // new decision starts here
                    accReward     = 0.f;
                    holdRemaining = 0;           // force new decision next frame
                }

                state = nextState;

                // Learn every N game frames once warmed up
                if (buffer.canSample(WARMUP_EXP) &&
                    (globalFrame % LEARN_EVERY == 0)) {
                    auto batch = buffer.sample(DQNAgent::BATCH_SIZE);
                    agent.learn(batch);
                }

                if (done) break;
            }

            if ((ep + 1) % PRINT_EVERY == 0 || ep == 0) {
                std::cout
                    << "Ep " << std::setw(4) << (ep + 1)
                    << " | L"   << (ep % 3 + 1)
                    << " | Decisions: " << std::setw(4) << decisionCount
                    << " | Reward: " << std::setw(8) << std::fixed
                    << std::setprecision(1) << episodeReward
                    << " | ε: " << std::setprecision(4) << agent.getEpsilon()
                    << " | Buf: " << buffer.size()
                    << "\n";
            }
        }

        agent.save_model(MODEL_PATH);
        std::cout << "\n[train] Done. Model saved → " << MODEL_PATH << "\n";

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
