#include <SFML/Graphics.hpp>
#include <vector>

int main() {
    const unsigned int windowWidth = 800;
    const unsigned int windowHeight = 600;
    const float penRadius = 2.0f;

    sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "SFML Whiteboard");
    //window.setFramerateLimit(120);
    
    std::vector<sf::CircleShape> strokes;
    bool drawing = false;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
                drawing = true;

            if (event->is<sf::Event::MouseButtonReleased>() && event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left)
                drawing = false;

            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::C) {
                strokes.clear(); // Press 'C' to clear the board
            }
        }

        if (drawing) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::CircleShape dot(penRadius);
            dot.setPosition({ mousePos.x - penRadius, mousePos.y - penRadius });
            dot.setFillColor(sf::Color::Black);
            strokes.push_back(dot);
        }

        window.clear(sf::Color::White);

        for (const auto& dot : strokes)
            window.draw(dot);

        window.display();
    }

    return 0;
}