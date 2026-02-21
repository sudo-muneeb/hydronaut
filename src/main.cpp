#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <algorithm>

#include "Constants.hpp"
#include "AssetManager.hpp"
#include "Menu.hpp"
#include "Level1.hpp"
#include "Level2.hpp"
#include "Level3.hpp"

int main() {
    // ─── Window ───────────────────────────────────────────────────────────────
    sf::RenderWindow window(
        sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT),
        "Hydronaut",
        sf::Style::Default   // resizable
    );
    window.setFramerateLimit(60);

    // ─── Enforce minimum size on first show ───────────────────────────────────
    window.setView(sf::View(sf::FloatRect(
        0, 0,
        (float)DEFAULT_WINDOW_WIDTH,
        (float)DEFAULT_WINDOW_HEIGHT)));

    // ─── Assets ───────────────────────────────────────────────────────────────
    try {
        AssetManager::instance().loadAll();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << std::endl;
        return -1;
    }

    // ─── Background music ─────────────────────────────────────────────────────
    sf::Music music;
    if (!music.openFromFile(ASSET_MUSIC)) {
        std::cerr << "[WARNING] Could not open music file." << std::endl;
    } else {
        music.setLoop(true);
        music.setVolume(10.f);
        music.play();
    }

    // ─── Main loop ────────────────────────────────────────────────────────────
    while (window.isOpen()) {
        Menu menu(window);
        int choice = menu.run();

        if (choice == -1 || !window.isOpen())
            break;

        std::unique_ptr<Level> level;
        if (choice == 1)      level = std::make_unique<Level1>(window);
        else if (choice == 2) level = std::make_unique<Level2>(window);
        else if (choice == 3) level = std::make_unique<Level3>(window);

        if (level) level->run();
    }

    return 0;
}
