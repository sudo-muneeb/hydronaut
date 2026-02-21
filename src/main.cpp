#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "Constants.hpp"
#include "AssetManager.hpp"
#include "Menu.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"

int main() {
    try {
        // ─── Window ───────────────────────────────────────────────────────
        sf::RenderWindow window(
            sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT),
            "Hydronaut",
            sf::Style::Default  // resizable
        );
        window.setFramerateLimit(60);
        window.setView(sf::View(sf::FloatRect(
            0.f, 0.f,
            static_cast<float>(DEFAULT_WINDOW_WIDTH),
            static_cast<float>(DEFAULT_WINDOW_HEIGHT))));

        // ─── Assets (throws on missing file) ──────────────────────────────
        AssetManager::instance().loadAll();

        // ─── Background music (non-fatal if missing) ──────────────────────
        sf::Music music;
        bool musicOk = music.openFromFile(ASSET_MUSIC);
        if (musicOk) {
            music.setLoop(true);
            music.setVolume(10.f);
            music.play();
        } else {
            std::cerr << "[WARNING] Music file not found: " << ASSET_MUSIC << "\n";
        }

        // ─── Main game loop ───────────────────────────────────────────────
        while (window.isOpen()) {
            Menu menu(window);
            int choice = menu.run();

            if (choice == -1 || !window.isOpen())
                break;

            std::unique_ptr<Level> level;
            switch (choice) {
                case 1: level = std::make_unique<Level1>(window); break;
                case 2: level = std::make_unique<Level2>(window); break;
                case 3: level = std::make_unique<Level3>(window); break;
                default: break;
            }

            // Per-level guard: a level crash returns to menu instead of dying
            if (level) {
                try {
                    level->run();
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Level crashed: " << e.what() << "\n";
                    // Continue — show menu again
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "[FATAL] Unknown exception.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
