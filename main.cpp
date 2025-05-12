#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <iostream>
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

class CircleButton : public tgui::CanvasSFML {
public:
    using Ptr = std::shared_ptr<CircleButton>;

    CircleButton(float radius, sf::Color normalColor = sf::Color::Blue, sf::Color hoverColor = sf::Color::Green)
        : m_radius(radius), m_normalColor(normalColor), m_hoverColor(hoverColor), m_isHovered(false)
    {
        setSize({radius * 2, radius * 2});
        updateGraphics();
    }

    static Ptr create(float radius, sf::Color normalColor = sf::Color::Blue, sf::Color hoverColor = sf::Color::Green) {
        return std::make_shared<CircleButton>(radius, normalColor, hoverColor);
    }

    void setPosition(sf::Vector2f pos) {
        tgui::CanvasSFML::setPosition(tgui::Layout2d(pos));
    }

    sf::Color getNormalColor(){
        return m_normalColor;
    }

    sf::Color getHoverColor(){
        return m_hoverColor;
    }

    void setOutline(sf::Color outlineColor, float outlineThickness){
        m_outlineColor = outlineColor;
        m_outlineThickness = outlineThickness;
        setSize({(m_radius + outlineThickness) * 2, (m_radius + outlineThickness) * 2});
        setPosition({getPosition().x - m_outlineThickness + m_totalOffset, getPosition().y - m_outlineThickness + m_totalOffset}); // totalOffset term for correction
        m_totalOffset = m_outlineThickness;
        updateGraphics();
    }

    tgui::Signal& getSignal(tgui::String signalName) override {
        if (signalName == "Clicked")
            return m_signalClicked;
        return tgui::CanvasSFML::getSignal(signalName);
    }

protected:
    bool mouseOnWidget(tgui::Vector2f pos) const {
        float dx = pos.x - m_radius;
        float dy = pos.y - m_radius;
        return (dx * dx + dy * dy) <= (m_radius * m_radius);
    }

    void mouseEnteredWidget() override {
        m_isHovered = true;
        updateGraphics();
    }

    void mouseLeftWidget() override {
        m_isHovered = false;
        updateGraphics();
    }

    bool leftMousePressed(tgui::Vector2f) {
        return m_signalClicked.emit(this);
    }

private:
    void updateGraphics() {
        clear(sf::Color::Transparent);
        sf::CircleShape circle(m_radius);
        circle.setPointCount(100); // Smooth circle
        circle.setFillColor(m_isHovered ? m_hoverColor : m_normalColor);
        circle.setPosition({m_outlineThickness, m_outlineThickness});
        circle.setOutlineThickness(m_outlineThickness);
        circle.setOutlineColor(m_outlineColor);
        draw(circle);
        display();
    }

    float m_radius;
    float m_outlineThickness = 0;
    bool m_isHovered;
    sf::Color m_outlineColor;
    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    float m_totalOffset = 0;
    tgui::Signal m_signalClicked{"Clicked"};
};

int main() {
    const unsigned int windowWidth = 800;
    const unsigned int windowHeight = 600;
    const float spacing = 5.0f;
    const float lineThickness = 10.0f;
    sf::Color strokeColor = sf::Color::Black;
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Pizarra", sf::Style::Default, sf::State::Windowed, settings);
    std::pair<sf::Vector2i, sf::Vector2i> boardBounds = {{0, 0}, {windowWidth, windowHeight - 100}};
    window.setFramerateLimit(120);


    std::vector<std::pair<std::vector<sf::Vector2f>, sf::Color>> allStrokePoints;
    std::vector<sf::Vector2f> currentStroke;
    bool drawing = false;
    sf::Vector2f lastPos;
    tgui::Gui gui{window};

    auto brushPanel = tgui::Panel::create({windowWidth, 100});
    brushPanel->setPosition({0, windowHeight - 100});
    // Set background color via renderer
    brushPanel->getRenderer()->setBackgroundColor(tgui::Color(32, 32, 32)); 

    sf::Color brushColors[] = {sf::Color::Black, sf::Color::White, sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Blue, sf::Color::Magenta};
    CircleButton::Ptr brushButtons[sizeof(brushColors)/sizeof(sf::Color)];
    gui.add(brushPanel);
    for (int i = 0; i < sizeof(brushColors)/sizeof(sf::Color); i++){
        auto button = CircleButton::create(20, brushColors[i], brushColors[i]);
        button->setPosition({windowWidth - 50*sizeof(brushColors)/sizeof(sf::Color) - 100 + i*50.f, (float)windowHeight - 70.f});
        button->getSignal("Clicked").connect([&brushButtons, i, brushColors, &strokeColor](){
            for (int j = 0; j < sizeof(brushColors)/sizeof(sf::Color); j++){
                brushButtons[j]->getSize();
                brushButtons[j]->setOutline(sf::Color::White, 0);
            }
            brushButtons[i]->setOutline(sf::Color::White, 3);
            strokeColor = brushButtons[i]->getNormalColor();
        });
        brushButtons[i] = button;
        gui.add(button);
    } 
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                window.close();

            if (event->is<sf::Event::MouseButtonPressed>() && event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (mousePos.x >= boardBounds.first.x && mousePos.x <= boardBounds.second.x && mousePos.y >= boardBounds.first.y && mousePos.y <= boardBounds.second.y){
                    drawing = true;
                } else {
                    drawing = false;
                }
                currentStroke.clear();
                lastPos = mousePos;
                currentStroke.push_back(lastPos);
            }

            if (event->is<sf::Event::MouseButtonReleased>() && event->getIf<sf::Event::MouseButtonReleased>()->button == sf::Mouse::Button::Left) {
                drawing = false;
                if (currentStroke.size() >= 2) {
                    allStrokePoints.push_back(std::pair<std::vector<sf::Vector2f>, sf::Color>(currentStroke, strokeColor));
                }
            }

            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::C) {
                allStrokePoints.clear();
                currentStroke.clear();
            }
        }
        
        if (drawing) {
            sf::Vector2f currentPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (!(currentPos.x >= boardBounds.first.x && currentPos.x <= boardBounds.second.x && currentPos.y >= boardBounds.first.y && currentPos.y <= boardBounds.second.y)){
                drawing = false;
            }
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
            for (size_t i = 1; i < stroke.first.size(); ++i) {
                std::vector<sf::Vertex> tri = createThickLine(stroke.first[i - 1], stroke.first[i], lineThickness, stroke.second);
                window.draw(tri.data(), tri.size(), sf::PrimitiveType::Triangles);
            }
        }

        // Draw current stroke in progress
        for (size_t i = 1; i < currentStroke.size(); ++i) {
            std::vector<sf::Vertex> tri = createThickLine(currentStroke[i - 1], currentStroke[i], lineThickness, strokeColor);
            window.draw(tri.data(), tri.size(), sf::PrimitiveType::Triangles);
        }
        gui.draw();
        window.display();
    }

    return 0;
}