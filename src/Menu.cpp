#include "Menu.hpp"
#include "AssetManager.hpp"
#include "Constants.hpp"

static constexpr int NUM_ITEMS = 3;
static const char*   ITEM_LABELS[NUM_ITEMS] = {
    "Obstacles Unleashed",
    "Arc of Chaos",
    "Waves of Danger"
};

// ─── Helper: logical view size (never use window.getSize() for world coords) ──
static sf::Vector2f vSize(const sf::RenderWindow& w) {
    return w.getView().getSize();
}

Menu::Menu(sf::RenderWindow& window)
    : m_window(window), m_font(AssetManager::instance().font())
{
    // Sync the view to the actual current window size — same fix as Level.
    auto sz = m_window.getSize();
    m_window.setView(sf::View(sf::FloatRect(
        0.f, 0.f, static_cast<float>(sz.x), static_cast<float>(sz.y))));

    for (int i = 0; i < NUM_ITEMS; ++i) {
        m_items[i].setFont(m_font);
        m_items[i].setString(ITEM_LABELS[i]);
        m_items[i].setOutlineColor(sf::Color(0, 0, 60));
        m_items[i].setOutlineThickness(2.f);
    }

    m_highlight.setFillColor(sf::Color(255, 255, 255, 50));
    m_highlight.setOutlineColor(sf::Color(100, 200, 255, 180));
    m_highlight.setOutlineThickness(2.f);

    layout();         // position items based on view size
    updateHighlight();
}

// ─── Re-layout all items relative to the current VIEW size ───────────────────
void Menu::layout() {
    sf::Vector2f vs   = vSize(m_window);
    unsigned itemSize = static_cast<unsigned>(vs.y * 0.06f);
    itemSize          = std::max(18u, itemSize);

    float spacing = vs.y * 0.12f;
    float startY  = vs.y * 0.42f;

    for (int i = 0; i < NUM_ITEMS; ++i) {
        m_items[i].setCharacterSize(itemSize);
        sf::FloatRect b = m_items[i].getLocalBounds();
        m_items[i].setOrigin(b.width / 2.f, b.height / 2.f);
        m_items[i].setPosition(vs.x / 2.f, startY + i * spacing);
    }
}

void Menu::updateHighlight() {
    for (int i = 0; i < NUM_ITEMS; ++i)
        m_items[i].setFillColor(i == m_selected
            ? sf::Color(100, 220, 255)
            : sf::Color(180, 220, 255, 160));

    sf::FloatRect b = m_items[m_selected].getGlobalBounds();
    m_highlight.setSize(sf::Vector2f(b.width + 30.f, b.height + 14.f));
    m_highlight.setPosition(b.left - 15.f, b.top - 7.f);
}

// ─── Background — uses VIEW size, not physical window size ───────────────────
void Menu::drawBackground() {
    sf::Vector2f vs = vSize(m_window);

    sf::VertexArray bg(sf::Quads, 4);
    bg[0] = sf::Vertex(sf::Vector2f(0,    0    ), sf::Color(0,  10,  50));
    bg[1] = sf::Vertex(sf::Vector2f(vs.x, 0    ), sf::Color(0,  10,  50));
    bg[2] = sf::Vertex(sf::Vector2f(vs.x, vs.y ), sf::Color(0,  80, 140));
    bg[3] = sf::Vertex(sf::Vector2f(0,    vs.y ), sf::Color(0,  80, 140));
    m_window.draw(bg);

    float hScale = vs.y / 900.f;

    // ─── Title ────────────────────────────────────────────────────────────
    sf::Text title;
    title.setFont(m_font);
    title.setCharacterSize(std::max(24u, static_cast<unsigned>(vs.y * 0.10f)));
    title.setFillColor(sf::Color(100, 220, 255));
    title.setOutlineColor(sf::Color(0, 0, 80));
    title.setOutlineThickness(3.f * hScale);
    title.setString("HYDRONAUT");
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.width / 2.f, tb.height / 2.f);
    title.setPosition(vs.x / 2.f, vs.y * 0.18f);
    m_window.draw(title);

    // ─── Subtitle ─────────────────────────────────────────────────────────
    sf::Text sub;
    sub.setFont(m_font);
    sub.setCharacterSize(std::max(12u, static_cast<unsigned>(vs.y * 0.033f)));
    sub.setFillColor(sf::Color(180, 240, 255, 200));
    sub.setString("Navigate the deep. Survive the chaos.");
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setOrigin(sb.width / 2.f, sb.height / 2.f);
    sub.setPosition(vs.x / 2.f, vs.y * 0.30f);
    m_window.draw(sub);

    // ─── Controls hint ────────────────────────────────────────────────────
    sf::Text hint;
    hint.setFont(m_font);
    hint.setCharacterSize(std::max(11u, static_cast<unsigned>(vs.y * 0.024f)));
    hint.setFillColor(sf::Color(120, 180, 220, 160));
    hint.setString("UP/DOWN: navigate     ENTER: select     ESC in-game: return");
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.width / 2.f, hb.height / 2.f);
    hint.setPosition(vs.x / 2.f, vs.y * 0.92f);
    m_window.draw(hint);
}

int Menu::run() {
    while (m_window.isOpen()) {
        // Re-layout every frame so resize is handled deterministically.
        layout();
        updateHighlight();

        sf::Event event;
        while (m_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return -1;

            if (event.type == sf::Event::Resized) {
                // Keep logical view in sync with whatever the OS reports.
                unsigned w = event.size.width;
                unsigned h = event.size.height;
                m_window.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)w, (float)h)));
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up)
                    m_selected = (m_selected - 1 + NUM_ITEMS) % NUM_ITEMS;
                else if (event.key.code == sf::Keyboard::Down)
                    m_selected = (m_selected + 1) % NUM_ITEMS;
                else if (event.key.code == sf::Keyboard::Enter)
                    return m_selected + 1;

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
