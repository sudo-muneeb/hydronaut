#include "HumanTrainer.hpp"
#include "DQNAgent.hpp"
#include <iostream>
#include <filesystem>

static const char* MODEL_PATH = "hydronaut_dqn.pt";
static const int   LEARN_EVERY = 4;
static const int   SAVE_EVERY  = 1000; // frames
static const int   WARMUP_EXP  = 64;

HumanTrainer& HumanTrainer::instance() {
    static HumanTrainer instance;
    return instance;
}

HumanTrainer::HumanTrainer() = default;
HumanTrainer::~HumanTrainer() {
    saveModel();
}

void HumanTrainer::initIfNeeded() {
    if (m_initialized) return;

    m_agent  = std::make_unique<DQNAgent>();
    m_buffer = std::make_unique<ReplayBuffer>(50000);

    // Try to load user's existing trained weights
    if (std::filesystem::exists(MODEL_PATH)) {
        m_agent->load_model(MODEL_PATH);
        std::cout << "[HumanTrainer] Loaded existing model: " << MODEL_PATH << "\n";
    } else {
        std::cout << "[HumanTrainer] No existing model found. Will train from scratch.\n";
    }
    
    // Low epsilon since we're mostly learning off-policy from a human,
    // but the DQN structure still needs it for internal Q-updates.
    m_agent->setEpsilon(0.01f);
    m_initialized = true;
}

void HumanTrainer::recordExperience(
    const std::vector<float>& state,
    int action,
    float reward,
    const std::vector<float>& next_state,
    bool done
) {
    if (action < 0 || action > 4) return; // Ignore invalid actions (like dash only)

    initIfNeeded();

    m_buffer->push(state, action, reward, next_state, done);
    m_frameCount++;

    if (m_buffer->size() > WARMUP_EXP && (m_frameCount % LEARN_EVERY == 0)) {
        m_agent->learn(*m_buffer);
    }

    if (done || (m_frameCount % SAVE_EVERY == 0)) {
        saveModel();
    }
}

void HumanTrainer::saveModel() {
    if (m_initialized && m_agent) {
        m_agent->save_model(MODEL_PATH);
        std::cout << "[HumanTrainer] Saved model (" << m_frameCount << " frames trained).\n";
    }
}
