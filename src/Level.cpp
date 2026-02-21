#include "Level.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"
#include <SFML/Graphics.hpp>
#include <string>

Level::Level(sf::RenderWindow& window) : m_window(window) {
    auto& font = AssetManager::instance().font();

    m_scoreText.setFont(font);
    m_scoreText.setCharacterSize(32);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setOutlineColor(sf::Color(0, 0, 80));
    m_scoreText.setOutlineThickness(2.f);

    m_pauseText.setFont(font);
    m_pauseText.setCharacterSize(60);
    m_pauseText.setFillColor(sf::Color::White);
    m_pauseText.setOutlineColor(sf::Color(0, 0, 0));
    m_pauseText.setOutlineThickness(3.f);
    m_pauseText.setString("PAUSED\nPress P to Resume");
}

int Level::run() {
    while (m_window.isOpen() && !m_gameOver) {
        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window.close();
                return m_score;
            }
            if (event.type == sf::Event::Resized) {
                // Enforce minimum window size
                unsigned int w = std::max(event.size.width,  (unsigned int)MIN_WINDOW_WIDTH);
                unsigned int h = std::max(event.size.height, (unsigned int)MIN_WINDOW_HEIGHT);
                m_window.setSize(sf::Vector2u(w, h));
                sf::FloatRect view(0, 0, (float)w, (float)h);
                m_window.setView(sf::View(view));
            }
            handlePauseKey(event);
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape) {
                return m_score; // back to menu
            }
        }

        if (m_paused) {
            // Draw pause overlay
            drawBackground();
            draw();
            // Dim overlay
            sf::RectangleShape dim(sf::Vector2f(
                (float)m_window.getSize().x, (float)m_window.getSize().y));
            dim.setFillColor(sf::Color(0, 0, 0, 160));
            m_window.draw(dim);
            // Center pause text
            sf::FloatRect pb = m_pauseText.getLocalBounds();
            m_pauseText.setOrigin(pb.width / 2.f, pb.height / 2.f);
            m_pauseText.setPosition(m_window.getSize().x / 2.f,
                                    m_window.getSize().y / 2.f);
            m_window.draw(m_pauseText);
            m_window.display();
            continue;
        }

        m_window.clear();
        drawBackground();

        if (update()) {
            m_gameOver = true;
        }

        draw();
        drawHUD();
        m_window.display();
        sf::sleep(sf::milliseconds(10));
    }

    showGameOver();
    return m_score;
}

void Level::handlePauseKey(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::P && !m_pKeyDown) {
        m_paused   = !m_paused;
        m_pKeyDown = true;
    }
    if (event.type == sf::Event::KeyReleased &&
        event.key.code == sf::Keyboard::P) {
        m_pKeyDown = false;
    }
}

void Level::drawBackground() {
    // Deep-sea blue gradient drawn procedurally — no image required
    auto size = m_window.getSize();
    sf::VertexArray bg(sf::Quads, 4);
    bg[0] = sf::Vertex(sf::Vector2f(0,              0             ), sf::Color(0,  15, 60));
    bg[1] = sf::Vertex(sf::Vector2f((float)size.x,  0             ), sf::Color(0,  15, 60));
    bg[2] = sf::Vertex(sf::Vector2f((float)size.x, (float)size.y  ), sf::Color(0,  60, 120));
    bg[3] = sf::Vertex(sf::Vector2f(0,             (float)size.y  ), sf::Color(0,  60, 120));
    m_window.draw(bg);
}

void Level::drawHUD() {
    m_scoreText.setString("Score: " + std::to_string(m_score));
    m_scoreText.setPosition(10.f, 10.f);
    m_window.draw(m_scoreText);
}

void Level::showGameOver() {
    sf::sleep(sf::milliseconds(400));

    auto& font = AssetManager::instance().font();
    sf::Text goText;
    goText.setFont(font);
    goText.setCharacterSize(56);
    goText.setFillColor(sf::Color(255, 80, 80));
    goText.setOutlineColor(sf::Color(0, 0, 0));
    goText.setOutlineThickness(3.f);
    goText.setString("Game Over!\nScore: " + std::to_string(m_score));

    sf::FloatRect tb = goText.getLocalBounds();
    goText.setOrigin(tb.width / 2.f, tb.height / 2.f);
    goText.setPosition(m_window.getSize().x / 2.f,
                       m_window.getSize().y / 2.f);

    m_window.clear();
    drawBackground();
    m_window.draw(goText);
    m_window.display();
    sf::sleep(sf::seconds(2.f));
}

void Level::addScore(int amount) { m_score += amount; }
