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
#include "Settings.hpp"
#include "HumanTrainer.hpp"

int main() {
    try {
        // ─── Window at full desktop resolution ────────────────────────────
        // Use the actual screen size at runtime so the game always fills the
        // display on any monitor without hardcoded dimensions.
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

        sf::RenderWindow window(
            desktop,
            "Hydronaut",
            sf::Style::Default   // resizable, decorated
        );
        window.setFramerateLimit(60);

        // Set the logical view to match the physical window exactly.
        window.setView(sf::View(sf::FloatRect(
            0.f, 0.f,
            static_cast<float>(desktop.width),
            static_cast<float>(desktop.height))));

        // ─── Assets ───────────────────────────────────────────────────────
        AssetManager::instance().loadAll();

        // ─── Background music ─────────────────────────────────────────────
        sf::Music music;
        bool musicOk = music.openFromFile(ASSET_MUSIC);
        if (musicOk) {
            music.setLoop(true);
            auto& s = Settings::instance();
            music.setVolume(s.isMuted() ? 0.f : s.getVolume());
            music.play();
        } else {
            std::cerr << "[WARNING] Music file not found: " << ASSET_MUSIC << "\n";
        }

        // ─── Main game loop ───────────────────────────────────────────────
        while (window.isOpen()) {
            Menu menu(window, musicOk ? &music : nullptr);
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

            if (level) {
                try {
                    level->run();
                } catch (const std::exception& e) {
                    std::cerr << "[ERROR] Level crashed: " << e.what() << "\n";
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

    // Safely shutdown HumanTrainer (and its PyTorch models) before exit
    HumanTrainer::instance().shutdown();

    return EXIT_SUCCESS;
}
