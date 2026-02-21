# Hydronaut 🚢

A 2D submarine dodge-and-collect game built with **SFML 2.x** and **C++17**.  
Navigate your submarine through increasingly chaotic obstacle patterns across 3 levels.

---

## Building & Running

```bash
bash run.sh
```

That's it. The script compiles everything and launches the game.

**Dependencies:** `libsfml-graphics`, `libsfml-window`, `libsfml-system`, `libsfml-audio`  
**Compiler:** g++ with `-std=c++17`

---

## Controls

| Key | Action |
|---|---|
| `↑ ↓ ← →` | Move submarine |
| `P` | Pause / Resume |
| `Escape` | Return to menu from any level |
| `Enter` | Select menu option |

---

## Project Structure

```
hydronaut/
├── assets/                  Media files (fonts, music, sprites)
├── include/                 Header files (one per class)
├── src/                     Source files (one per class)
├── run.sh                   Build + run script
└── hydronaut                Compiled binary
```

---

## File Descriptions

### `include/Constants.hpp`
Global compile-time constants:
- `DEFAULT_WINDOW_WIDTH / HEIGHT` — initial window size (900×900)
- `MIN_WINDOW_WIDTH / HEIGHT` — resize floor (640×480), enforced at runtime
- `PLAYER_SPEED`, `INITIAL_OBSTACLE_SPEED`, etc. — gameplay tuning knobs
- `ASSET_*` — all asset paths in one place

---

### `include/AssetManager.hpp` / `src/AssetManager.cpp`
**Singleton** that loads every texture and the font **once** on startup.

- `AssetManager::instance().loadAll()` — throws `std::runtime_error` if any asset is missing (fast-fail)
- `texture("submarine")` / `font()` — O(1) lookup via `unordered_map`
- All SFML objects are kept alive for the entire process lifetime — no dangling texture pointers

---

### `include/Player.hpp` / `src/Player.cpp`
The **player submarine**.

- Scaled to 7% of window height, starts at the left-center
- `handleInput(windowSize)` — reads arrow keys, clamps position to window bounds
- `reset(windowSize)` — repositions for a new level without reallocating
- Position clamping is fully relative to the live `window.getSize()`

---

### `include/Obstacle.hpp`
**Abstract base class** for all enemies. Defines the interface:
```cpp
virtual void update(sf::Vector2u windowSize) = 0;
virtual void draw(sf::RenderWindow& window)  = 0;
virtual sf::FloatRect getBounds() const      = 0;
virtual void reset(sf::Vector2u windowSize)  = 0;
```
All implementations receive the live window size each frame — the math functions normalize
`x` and `y` to fractions of `windowSize`, so every level adapts automatically to any resolution.

---

### `include/ConvexObstacle.hpp` / `src/ConvexObstacle.cpp`  *(Level 1)*
Orange rotating triangles.

- `ConvexObstacle` — individual spinning triangle; size = 10% of `min(width, height)`
- `ConvexObstaclePool` — manages a `std::vector` pre-reserved to 30 slots to avoid mid-frame reallocations; spawns with ~1-in-50 random chance per frame; removes off-screen obstacles
- Speed starts at `INITIAL_OBSTACLE_SPEED` and grows as `speed = base × 1.3^(score/100)` up to score 500

### `include/SineObstacle.hpp` / `src/SineObstacle.cpp`  *(Level 2)*
Sea-urchin that traces a sine wave.

**Math (window-relative):**
```
half = windowHeight / 2
if x ≤ W/2:  y = half × (1 - sin(π/W × x))
else:        y = half + |sin(π/W × (x - W/2))| × half
```
Moves left at 3 px/frame; respawns in the right 50% of the screen.

### `include/ParabolicObstacle.hpp` / `src/ParabolicObstacle.cpp`  *(Level 2)*
Crab that bounces along a parabolic arc.

**Math (window-relative):**
```
norm = x / windowWidth        (0..1)
para = 4 × norm × (1 - norm)  (peaks at 1 when centered)
y = half ± half × √para
```
Direction flips at x=0 (bounce back right) and at x=windowWidth (resets).

### `include/SecObstacle.hpp` / `src/SecObstacle.cpp`  *(Level 3)*
Octopus following a secant curve.

**Math (window-relative):**
```
angle = (π / windowWidth) × 3 × x
y = sec(angle) / 6 × windowHeight/2   (clamped to avoid asymptotes)
```
Moves left at 1 px/frame (slowest — hardest to predict). Asymptote guard: if |cos| < 0.05, clamp to 0.05.

### `include/ExpSineObstacle.hpp` / `src/ExpSineObstacle.cpp`  *(Level 3)*
Lanternfish on an exponentially damped sine path.

**Math (window-relative):**
```
amplitude = windowHeight × 0.15
y = windowHeight/2 + amplitude × sin(0.01×x) × exp(-0.001×x)
```
Oscillations shrink as x increases; resets to right side when it reaches the left edge.

---

### `include/Treasure.hpp` / `src/Treasure.cpp`
Treasure chest that appears in Levels 2 and 3.

- Spawns within the **inner 70%** of the window (15–85% margin on each axis)
- `respawn(windowSize)` — repositions to a new random location and rescales to window
- Guard against inverted spawn ranges on very small windows

---

### `include/Level.hpp` / `src/Level.cpp`
**Abstract base class** for all 3 levels. Owns the shared game loop:

```
run() loop
 ├─ pollEvent → resize enforcement, P pause, Escape to menu
 ├─ drawBackground() — deep-sea gradient quad, no image required
 ├─ update() [pure virtual] → obstacle logic, collision detection
 ├─ draw()   [pure virtual] → draw enemies, player, treasure
 └─ drawHUD()              → score text top-left
showGameOver()             → overlay, 2-second display
```
- Resize events clamp window to `MIN_WINDOW_WIDTH × MIN_WINDOW_HEIGHT`
- Pause draws a dim overlay + "PAUSED" text and keeps the scene visible beneath

### `include/Level1.hpp` / `src/Level1.cpp` — *Obstacles Unleashed*
- Uses `ConvexObstaclePool`
- Score increments **every frame** (survival-based)
- Obstacle speed recalculates each frame from `getScore()`

### `include/Level2.hpp` / `src/Level2.cpp` — *Arc of Chaos*
- `SineObstacle` (urchin) + `ParabolicObstacle` (crab)
- Score += 10 for each **treasure** collected
- Touch either obstacle → game over

### `include/Level3.hpp` / `src/Level3.cpp` — *Waves of Danger*
- `SecObstacle` (octopus) + `ExpSineObstacle` (lanternfish)
- Score += 10 per treasure
- Hardest movement patterns

---

### `include/Menu.hpp` / `src/Menu.cpp`
- Deep-sea gradient background drawn in code (navy → teal-blue) via `sf::VertexArray`
- Title "HYDRONAUT", subtitle, and controls hint drawn proportionally to window size
- Items and highlight rescale on every frame — correct after any resize
- Returns selected level (1/2/3) or -1 on close

---

### `src/main.cpp`
Entry point:
1. Creates an `sf::RenderWindow` (900×900, resizable)
2. Calls `AssetManager::instance().loadAll()` — **throws** on missing file
3. Opens music (non-fatal if missing)
4. Main loop: `Menu::run()` → instantiates the correct `Level` subclass as `unique_ptr<Level>` → `level->run()`
5. **Two-level exception handling:**
   - Inner catch around each level → crash returns to menu
   - Outer catch around everything → fatal error with message, clean exit

---

## Program Flow

```
main()
 └─ AssetManager::loadAll()       ←── throws on missing asset
 └─ [music loop]
 └─ while(window.isOpen())
      ├─ Menu::run()              ←── returns 1/2/3 or -1
      └─ unique_ptr<Level> level
           ├─ Level1 / Level2 / Level3 (constructor)
           │     └─ Player(windowSize)
           │     └─ [Obstacle subclasses](windowSize)
           │     └─ Treasure(windowSize)       [L2/L3 only]
           └─ level->run()                     ← base Level loop
                 ├─ sf::Event polling
                 ├─ drawBackground()
                 ├─ update()    ← derived class: move + collide
                 ├─ draw()      ← derived class: render enemies
                 └─ drawHUD()
           └─ showGameOver()
           └─ [back to menu]
```

---

## Exception Safety

| Scenario | Behaviour |
|---|---|
| Missing font or texture | `AssetManager::loadAll()` throws → fatal exit with message |
| Missing music | Warning printed; game continues without audio |
| Level throws at runtime | Caught by per-level guard; returns to menu |
| Any other crash | Outer `catch(...)` prints message and exits cleanly |
| Zero-size window | All obstacle `reset()` / `update()` methods early-return |
| Modulo-zero in rand | All `rand() % n` guarded so `n > 0` before call |

---

## Memory Notes

- All SFML resources (textures, font) are owned by `AssetManager` (static singleton) — no manual `delete`
- `ConvexObstaclePool` pre-reserves 30 slots on first use — no mid-frame vector reallocation
- Level objects are `unique_ptr<Level>` — automatically destroyed when returning to menu
- Sprites hold a `const sf::Texture*` pointer into `AssetManager` (which lives for the process) — no dangling pointers
