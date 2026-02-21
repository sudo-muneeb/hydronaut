#include "Menu.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"

static constexpr int NUM_ITEMS = 3;
static const char*   ITEM_LABELS[NUM_ITEMS] = {
    "Obstacles Unleashed",
    "Arc of Chaos",
    "Waves of Danger"
};

Menu::Menu(sf::RenderWindow& window)
    : m_window(window), m_font(AssetManager::instance().font())
{
    auto size = m_window.getSize();

    for (int i = 0; i < NUM_ITEMS; ++i) {
        m_items[i].setFont(m_font);
        m_items[i].setCharacterSize(static_cast<unsigned>(size.y * 0.06f));
        m_items[i].setString(ITEM_LABELS[i]);
        m_items[i].setOutlineColor(sf::Color(0, 0, 60));
        m_items[i].setOutlineThickness(2.f);
    }

    // Position items centered vertically in bottom 60% of screen
    float spacing = size.y * 0.12f;
    float startY  = size.y * 0.35f;
    for (int i = 0; i < NUM_ITEMS; ++i) {
        sf::FloatRect b = m_items[i].getLocalBounds();
        m_items[i].setOrigin(b.width / 2.f, b.height / 2.f);
        m_items[i].setPosition(size.x / 2.f, startY + i * spacing);
    }
    m_highlight.setFillColor(sf::Color(255, 255, 255, 50));
    m_highlight.setOutlineColor(sf::Color(100, 200, 255, 180));
    m_highlight.setOutlineThickness(2.f);
    updateHighlight();
}

void Menu::updateHighlight() {
    // Re-tint all items
    for (int i = 0; i < NUM_ITEMS; ++i)
        m_items[i].setFillColor(i == m_selected
            ? sf::Color(100, 220, 255)
            : sf::Color(180, 220, 255, 160));

    sf::FloatRect b = m_items[m_selected].getGlobalBounds();
    m_highlight.setSize(sf::Vector2f(b.width + 30.f, b.height + 14.f));
    m_highlight.setPosition(b.left - 15.f, b.top - 7.f);
}

void Menu::drawBackground() {
    auto size = m_window.getSize();

    // Deep ocean gradient: dark navy at top, teal-blue at bottom
    sf::VertexArray bg(sf::Quads, 4);
    bg[0] = sf::Vertex(sf::Vector2f(0,            0           ), sf::Color(0,  10,  50));
    bg[1] = sf::Vertex(sf::Vector2f((float)size.x, 0           ), sf::Color(0,  10,  50));
    bg[2] = sf::Vertex(sf::Vector2f((float)size.x,(float)size.y), sf::Color(0,  80, 140));
    bg[3] = sf::Vertex(sf::Vector2f(0,            (float)size.y), sf::Color(0,  80, 140));
    m_window.draw(bg);

    // Title
    sf::Text title;
    title.setFont(m_font);
    title.setCharacterSize(static_cast<unsigned>(size.y * 0.10f));
    title.setFillColor(sf::Color(100, 220, 255));
    title.setOutlineColor(sf::Color(0, 0, 80));
    title.setOutlineThickness(3.f);
    title.setString("HYDRONAUT");
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(size.x / 2.f, size.y * 0.18f);
    m_window.draw(title);

    // Subtitle
    sf::Text sub;
    sub.setFont(m_font);
    sub.setCharacterSize(static_cast<unsigned>(size.y * 0.033f));
    sub.setFillColor(sf::Color(180, 240, 255, 200));
    sub.setString("Navigate the deep. Survive the chaos.");
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setOrigin(sb.width / 2.f, sb.height / 2.f);
    sub.setPosition(size.x / 2.f, size.y * 0.28f);
    m_window.draw(sub);

    // Controls hint at bottom
    sf::Text hint;
    hint.setFont(m_font);
    hint.setCharacterSize(static_cast<unsigned>(size.y * 0.025f));
    hint.setFillColor(sf::Color(120, 180, 220, 160));
    hint.setString("UP/DOWN to navigate   ENTER to select   ESC in-game to return");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.width / 2.f, hb.height / 2.f);
    hint.setPosition(size.x / 2.f, size.y * 0.92f);
    m_window.draw(hint);
}

int Menu::run() {
    while (m_window.isOpen()) {
        // Refresh menu item positions if window resized
        auto size = m_window.getSize();
        float spacing = size.y * 0.12f;
        float startY  = size.y * 0.35f;
        for (int i = 0; i < NUM_ITEMS; ++i) {
            m_items[i].setCharacterSize(static_cast<unsigned>(size.y * 0.06f));
            sf::FloatRect b = m_items[i].getLocalBounds();
            m_items[i].setOrigin(b.width / 2.f, b.height / 2.f);
            m_items[i].setPosition(size.x / 2.f, startY + i * spacing);
        }
        updateHighlight();

        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return -1;

            if (event.type == sf::Event::Resized) {
                unsigned w = std::max(event.size.width,  (unsigned)MIN_WINDOW_WIDTH);
                unsigned h = std::max(event.size.height, (unsigned)MIN_WINDOW_HEIGHT);
                m_window.setSize(sf::Vector2u(w, h));
                m_window.setView(sf::View(sf::FloatRect(0, 0, (float)w, (float)h)));
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up)
                    m_selected = (m_selected - 1 + NUM_ITEMS) % NUM_ITEMS;
                else if (event.key.code == sf::Keyboard::Down)
                    m_selected = (m_selected + 1) % NUM_ITEMS;
                else if (event.key.code == sf::Keyboard::Enter)
                    return m_selected + 1; // 1, 2, or 3

                updateHighlight();
            }
        }

        m_window.clear();
        drawBackground();
        m_window.draw(m_highlight);
        for (int i = 0; i < NUM_ITEMS; ++i)
            m_window.draw(m_items[i]);
        m_window.display();
    }
    return -1;
}
