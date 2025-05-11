#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

sf::Vertex constructVertex(sf::Vector2f pos, sf::Color col){
    sf::Vertex vtx;
    vtx.position = pos;
    vtx.color = col;
    return vtx;
}

sf::Vector2f normalize(sf::Vector2f v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    return len != 0 ? v / len : sf::Vector2f(0, 0);
}

// Convert a line segment (a->b) into two triangles forming a thick line quad
std::vector<sf::Vertex> createThickLine(sf::Vector2f a, sf::Vector2f b, float thickness, sf::Color color) {
    sf::Vector2f direction = normalize(b - a);
    sf::Vector2f normal(-direction.y, direction.x);
    sf::Vector2f offset = normal * (thickness / 2.0f);

    sf::Vector2f p1 = a + offset;
    sf::Vector2f p2 = b + offset;
    sf::Vector2f p3 = b - offset;
    sf::Vector2f p4 = a - offset;

    return {
        constructVertex(p1, color), constructVertex(p2, color), constructVertex(p3, color),
        constructVertex(p3, color), constructVertex(p4, color), constructVertex(p1, color)
    };
}

int main() {
    const unsigned int windowWidth = 800;
    const unsigned int windowHeight = 600;
    const float spacing = 5.0f;
    const float lineThickness = 10.0f;

    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Pizarra");
    window.setFramerateLimit(120);

    std::vector<std::vector<sf::Vector2f>> allStrokePoints;
    std::vector<sf::Vector2f> currentStroke;
    bool drawing = false;
    sf::Vector2f lastPos;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                drawing = true;
                currentStroke.clear();
                lastPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                currentStroke.push_back(lastPos);
            }

            if (event->is<sf::Event::MouseButtonReleased>() && event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left) {
                drawing = false;
                if (currentStroke.size() >= 2) {
                    allStrokePoints.push_back(currentStroke);
                }
            }

            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::C) {
                allStrokePoints.clear();
                currentStroke.clear();
            }
        }

        if (drawing) {
            sf::Vector2f currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            float dist = std::hypot(currentPos.x - lastPos.x, currentPos.y - lastPos.y);
            int steps = std::max(1, static_cast<int>(dist / spacing));

            for (int i = 1; i <= steps; ++i) {
                float t = static_cast<float>(i) / steps;
                sf::Vector2f interp = lastPos + t * (currentPos - lastPos);
                currentStroke.push_back(interp);
            }

            lastPos = currentPos;
        }

        window.clear(sf::Color::White);

        // Draw all finished strokes
        for (const auto& stroke : allStrokePoints) {
            for (size_t i = 1; i < stroke.size(); ++i) {
                std::vector<sf::Vertex> tri = createThickLine(stroke[i - 1], stroke[i], lineThickness, sf::Color::Black);
                window.draw(tri.data(), tri.size(), sf::PrimitiveType::Triangles);
            }
        }

        // Draw current stroke in progress
        for (size_t i = 1; i < currentStroke.size(); ++i) {
            std::vector<sf::Vertex> tri = createThickLine(currentStroke[i - 1], currentStroke[i], lineThickness, sf::Color::Black);
            window.draw(tri.data(), tri.size(), sf::PrimitiveType::Triangles);
        }

        window.display();
    }

    return 0;
}