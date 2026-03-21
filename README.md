# Hydronaut 🚢

A 2D submarine dodge-and-collect game built with **SFML 2.x** and **C++17**, featuring a custom **Deep Q-Network (DQN) AI** trained via **LibTorch** to autonomously play the game!

Navigate your submarine through increasingly chaotic obstacle patterns across 3 distinct levels, or watch as the integrated neural network learns to dodge, survive, and hunt for treasure on its own.

---

## 🌟 Overview & Key Features

Hydronaut serves as both an entertaining survival/collection arcade game and a fully realized environment for Reinforcement Learning (RL). 

*   **Responsive 2D Engine**: Built robustly using C++17 and SFML, utilizing screen-relative mathematics allowing the game and the AI to scale seamlessly across any window resolution seamlessly.
*   **Three Distinct Challenges**:
    *   **Level 1 (Obstacles Unleashed):** Pure survival against aggressively swarming, rotating convex shapes.
    *   **Level 2 (Arc of Chaos):** Treasure hunting while dodging Sine-wave and Parabolic moving creatures.
    *   **Level 3 (Waves of Danger):** Advanced treasure hunting amidst enemies that stalk via complex Secant and Exponential-Sine mathematical curves.
*   **Integrated DQN Agent**: A robust Deep Reinforcement Learning agent completely capable of executing the game autonomously, built top-to-bottom using PyTorch's C++ library (LibTorch).
*   **Asset Management Singleton**: Ensures reliable memory and resource management keeping SFML textures and fonts protected from dangling pointers and reallocation mid-frame.

---

## 🧠 Deep Reinforcement Learning (AI)

At the heart of the project is a deep neural network that learned to conquer the game through thousands of simulated episodes without raw pixel data.

### Architecture
- **LibTorch C++ API**: Used to train the AI directly inside the C++ environment natively.
- **Model**: A multi-layer perceptron (MLP) mapping a 49-dimensional state space down to 5 action outputs (`Up, Down, Left, Right, Idle`). The architecture scales through 3 dense hidden layers (256 -> 256 -> 128).
- **Target Network & Experience Replay**: The agent stores 50,000 transition frames in a replay buffer, sampling random batches of 64 frames to gently stabilize Q-learning using delayed target soft-updates.

### State & Reward Shaping
- **Normalized Observations**: The AI is fed a tightly normalized (`[0, 1]` and `[-1, 1]`) 49-dimensional float array carrying game context: Level ID, viewport dimensions, specific velocities, timeout counters, and proximity coordinates for up to 8 objects.
- **Goal-Oriented AI**: Depending on the level, the AI is motivated by unique reward algorithms. Level 1 strictly rewards survival. Levels 2 & 3 heavily incentivize the approach vector towards treasure, punish camping/idling below certain speeds, and provide massive reward spikes (+200.0) for grabbing the chest while treating enemy collision as heavily lethal (-100.0).
- **Resolution Agnostic Training**: The AI logic is trained across dynamically shifting resolutions `(640x480 to 1920x1080)` to teach spatial adaptation rather than memorizing routes.

---

## 🚀 Building & Running

### Dependencies
- `libsfml-graphics`, `libsfml-window`, `libsfml-system`, `libsfml-audio`
- `g++` (Compiler with `-std=c++17` support)
- `LibTorch` / PyTorch C++ API (for AI components)
- `CMake` (for orchestrating the build)

### Quick Start

The included shell script will handle standard compilation and launch the game for human play:

```bash
bash run.sh
```

### Running the AI Executables
If the project is built via CMake, specific RL targets are produced:
*   `./build/play_hydronaut`: Opens the physical rendering window and runs the game in pure algorithmic exploitation using the trained `hydronaut_dqn.pt` model.
*   `./build/train_hydronaut`: Executes the game in rapid headless mode (no visual SFML window overhead), allowing the network to rapidly train policies and commit gradients across generations.

---

## 🎮 Controls (Human Mode)

| Key | Action |
|---|---|
| `↑ ↓ ← →` | Move the submarine |
| `P` | Pause / Resume |
| `Escape` | Return to main menu from any level / quit |
| `Enter` | Select menu option |

---

## 📂 Project Structure

```text
hydronaut/
├── assets/                  Media files (fonts, music, sprites)
├── include/                 Header files defining game objects & RL states
├── src/                     Source files (engine loop, physics, AI execution)
├── libtorch/                C++ PyTorch libraries used for DQN
├── CMakeLists.txt           Build definitions for Game, Train, and Play targets
├── run.sh                   Quick-start build/run script
└── hydronaut                Compiled main binary
```

**Memory & Safety Notes:**
All objects are strongly protected via double-exception catching per loop, window size safety clamps, strictly bound vectors replacing dynamic arrays per-frame, and deterministic memory teardown using `std::unique_ptr` per level execution.
