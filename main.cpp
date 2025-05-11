#include <SFML/Graphics.hpp>
#include <SelbaWard.hpp>
#include <vector>
sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, float t) {
    return a + t * (b - a);
}

float distance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
}

int main() {
    const unsigned int windowWidth = 800;
    const unsigned int windowHeight = 600;
    const float penRadius = 2.0f;
    const float spacing = 1.5f; // Distance between interpolated points
    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "SFML Whiteboard");
    //window.setFramerateLimit(120);
    
    std::vector<sf::Vertex> strokes;
    bool drawing = false;
    sf::Vector2f lastPos;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                drawing = true;
                lastPos = sf::Vector2f(sf::Mouse::getPosition(window));
            }

            if (event->is<sf::Event::MouseButtonReleased>() && event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left) {
                drawing = false;
            }

            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::C) {
                strokes.clear(); // Press 'C' to clear the board
            }
        }
        if (drawing) {
            sf::Vector2f currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            float dist = distance(lastPos, currentPos);
            int steps = std::max(1, static_cast<int>(dist / spacing));

            for (int i = 0; i < steps; ++i) {
                float t = static_cast<float>(i) / steps;
                sf::Vector2f interp = lerp(lastPos, currentPos, t);
                sf::Vertex vtx;
                vtx.position = interp;
                vtx.color = sf::Color::Black;
                strokes.push_back(vtx);
            }

            lastPos = currentPos;
        }
        window.clear(sf::Color::White);
        if (!strokes.empty())
            window.draw(&strokes[0], strokes.size(), sf::PrimitiveType::LineStrip);
        //window.draw(strokes.begin(), sf::Lines);
        //for (const auto& dot : strokes)
        //    window.draw(dot);

        window.display();
    }

    return 0;
}