# Hydronaut: Reinforcement Learning (DQN) AI Implementation

This document details the architecture and implementation of the Deep Q-Network (DQN) agent that learns to play Hydronaut. The AI is built using **LibTorch** (PyTorch's C++ API) and interacts with the game engine without modifying the core simulation loop.

---

## 🏗️ Architecture Overview

The AI implementation is composed of several key components:

### 1. The Environment API (`Level::step()`)
To train the AI without running the full visual game loop, the `Level` classes expose an RL-specific API:
- **`std::vector<float> reset(sf::Vector2u windowSize)`**: Resets the level state, randomizes positions, and returns the initial state vector.
- **`std::vector<float> step(int action, float& reward, bool& isDone)`**: Applies exactly one action, advances the physics simulation by one frame (`dt = 1/60s`), and returns the new state, the scalar reward, and a boolean indicating if the episode has ended (e.g. death).
- **`std::vector<float> getState() const`**: Computes the 49-dimensional normalized state vector representing the current frame.

### 2. The Agent (`DQNAgent.hpp` / `DQNAgent.cpp`)
The `DQNAgent` handles the neural network model, action selection (using $\epsilon$-greedy exploration), and the Q-learning update step.
- **Model**: A multi-layer perceptron (MLP) with 3 hidden layers (`49 -> 256 -> 256 -> 128 -> 5`).
- **Target Network**: A delayed copy of the main network used to stabilize the Q-learning targets (`polyak averaging / soft updates`).
- **Optimizer**: Adam optimizer with a learning rate of `1e-4`.

### 3. The Replay Buffer (`ReplayBuffer.hpp`)
Experience Replay is used to break correlation between consecutive frames and allow the agent to learn from past experiences.
- **Capacity**: Stores the last `50,000` transitions `(state, action, reward, next_state, done)`.
- **Sampling**: Uniformly samples batches of size `64` for training.

### 4. Advanced RL Engine Architecture (GoF Patterns)
To support robust telemetry gathering, imitation learning, and rapid RL episodic resets, the simulation engine is strictly decoupled using pure Gang of Four (GoF) design patterns:
- **Dependency Injection**: The core `GameApp` and all `SimulationEnvironment` game logic are isolated from concrete SFML rendering dependencies or the RL `HumanTrainer` singleton via pure abstract interfaces (`IInterfaces.hpp` and `SFMLAdapters.hpp`).
- **Command Pattern**: All physical inputs are decoupled from the simulation. The `InputHandler` generates discrete `Command` objects pushed to a `CommandQueue`, allowing flawless serialization to `telemetry.log` for imitation learning. 
- **Observer Pattern**: A Publish-Subscribe `EventBus` manages communications between core physics, telemetry loggers, and reward systems. `RewardObserver`, `ScoreObserver`, and `TrainerObserver` react to standard events uniformly (`RewardGranted`, `PlayerCollision`, `ExperienceGenerated`) without hardcoded linkages.
- **State & Strategy Patterns**: The `HydronautEntity` flattens complex branching logic by delegating behavior directly to discrete State objects (`NormalState`, `DashState`). Meanwhile, `SimulationEnvironment` accepts injected `IRewardStrategy` implementations (`Level1RewardStrategy`, etc.) to dynamically swap score calculations.
- **Memento Pattern**: To avoid the heavy computational penalty of destroying and recreating the physics object graph during RL episodes, `SimulationEnvironment` acts as an Originator, exposing `create_memento()` and `restore_memento()` to rapidly snapshot and rollback the precise mathematical game state isolated inside an opaque `SimulationMemento` container.

---

## 🧠 State Representation (49 Dimensions)

The neural network receives a `49-dimensional` floating-point vector, strictly normalized down to `[0, 1]` or `[-1, 1]` to aid neural network convergence.

| Index(es) | Description | Normalization |
| :--- | :--- | :--- |
| `0` | Level ID (0.0 for L1, 0.5 for L2, 1.0 for L3) | `[0, 1]` |
| `1, 2` | Current screen width / height (compared to 1920x1080) | `[0, 1]` |
| `3, 4` | Player X / Y coordinates | `[0, 1]` |
| `5, 6` | Player Velocity X / Y | `[-1, 1]` |
| `7` | Steps since last reward (timeout counter) | `[0, 1]` |
| `8` | Value of the last reward received | `[-1, 1]` |
| `9...48` | Object Slots (up to 8 slots, 5 floats per slot) | mixed |

**Object Slot Layout (5 floats per object):**
- `[0]` Object Type ID (scaled `1/6` to `6/6`)
- `[1, 2]` Object X / Y coordinates `[0, 1]`
- `[3, 4]` Object Velocity X / Y `[-1, 1]`

---

## 🎮 Action Space

The agent outputs a discrete action integer from `0` to `4`, matching the human input mapped via `InputHandler`.
- `0`: Up
- `1`: Down
- `2`: Left
- `3`: Right
- `4`: None (Idle)

*Note: The AI cannot use the spacebar "Dash" ability. It must navigate purely using standard movement.*

---

## 🏆 Reward Shaping

The reward functions vary slightly by level to guide the agent toward the exact behaviors we want to see.

### Level 1 (Survival)
The goal is simply to survive the swarm of triangles for as long as possible.
- **+1.0**: For every frame survived.
- **-100.0**: Upon collision with a triangle (Lethal).

### Level 2 & 3 (Treasure Hunting)
The agent must actively navigate the screen to catch the treasure chest while avoiding complex boss enemies. To prevent the agent from finding safe "blind spots" and camping indefinitely, the reward is heavily shaped:
- **+0.15**: Base survival reward per frame.
- **+Approach (distance_closed * 0.50)**: Strong positive reinforcement for moving *closer* to the treasure.
- **-0.50 (Idle Penalty)**: If the agent's speed drops below `0.5f`, it is punished to prevent camping.
- **+200.0**: Massive spike reward upon successfully intersecting the treasure chest.
- **-100.0**: Upon collision with an enemy (Lethal).

---

## 🚀 Training & Inference Pipelines

### Training (`src/train.cpp`)
- **Headless Execution**: Runs invisibly without rendering actual pixels.
- **Randomized Resolution**: The virtual screen size randomizes between `640x480` and `1920x1080` every episode so the agent learns relative space instead of memorizing fixed pixel routes.
- **Round-Robin Episodes**: Continually cycles through Level 1, 2, and 3 during training so a single unified model (`hydronaut_dqn.pt`) can master all game modes.
- **Action Hold**: The fast simulation speed (`1/60s`) makes frame-by-frame action selection too chaotic. The agent selects an action and "holds" it for a random duration between `20-50` frames, mimicking human reaction times.

### Playback (`src/play.cpp`)
- Loads the trained `hydronaut_dqn.pt` model.
- Hooks into the SFML window, physically rendering every frame.
- Uses `epsilon = 0.0` (pure exploitation) to navigate the level using the learned weights.
