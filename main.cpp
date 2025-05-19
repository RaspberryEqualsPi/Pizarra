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
    return len != 0 ? v / len : sf::Vector2f(0, 0); // return 0 vector if length is 0, otherwise conver it to unit vector
}
class CircleButton : public tgui::CanvasSFML {
public:
    using Ptr = std::shared_ptr<CircleButton>;

    CircleButton(float radius, sf::Color normalColor = sf::Color::Blue, sf::Color hoverColor = sf::Color::Green)
        : m_radius(radius), m_normalColor(normalColor), m_hoverColor(hoverColor), m_isHovered(false)
    {
        setSize({radius * 2, radius * 2});
        setFocusable(true);
        onMouseEnter([this](){
            m_isHovered = true;
            updateGraphics();
        });
        onMouseLeave([this](){
            m_isHovered = false;
            updateGraphics();
        });
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

    void setNormalColor(sf::Color normalColor){
        m_normalColor = normalColor;
        updateGraphics();
    }

    void setHoverColor(sf::Color hoverColor){
        m_hoverColor = hoverColor;
        updateGraphics();
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
    bool isMouseOnWidget(tgui::Vector2f pos) const override {
        tgui::Vector2f mouseCoords = {pos.x - getPosition().x, pos.y - getPosition().y}; // make the mouse coordinates relative
        //std::cout << "mouseOnWidget called at: " << mouseCoords.x << ", " << mouseCoords.y << "\n";
        float dx = mouseCoords.x - (m_radius + m_outlineThickness);
        float dy = mouseCoords.y - (m_radius + m_outlineThickness);
        return ((dx * dx + dy * dy) <= (m_radius * m_radius));
    }

    /*void mouseEnteredWidget() override {

    }*/

    /*void mouseLeftWidget() override {

    }*/

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

class DrawingCanvas : public tgui::CanvasSFML {
public:
    using Ptr = std::shared_ptr<DrawingCanvas>;

    DrawingCanvas(sf::RenderWindow* realWindow, sf::Color strokeColor, float lineThickness, float spacing)
        : m_realWindow(realWindow), m_strokeColor(strokeColor), m_lineThickness(lineThickness), m_spacing(spacing)
    {
        this->onMouseEnter([this]() {
            m_mouseOnCanvas = true;
        });
        this->onMouseLeave([this]() {
            m_mouseOnCanvas = false;
        });
        onMousePress([this](){
            //this->setFocused(true);
            std::cout << "focusing\n";
            sf::Vector2i rawMousePos = sf::Mouse::getPosition(*m_realWindow);
            sf::Vector2f mousePos = this->mapPixelToCoords({ (float)(rawMousePos.x - getPosition().x), (float)(rawMousePos.y - getPosition().y) }); // take difference to make the position relative
            m_drawing = true;
            m_currentStroke.clear();
            m_currentStrokeTriangles.clear();
            m_lastPos = mousePos;
            m_currentStroke.push_back(m_lastPos);
        });
        onMouseRelease([this](){
            m_drawing = false;

            // Draw current stroke to cached texture
            if (m_currentStrokeTriangles.getVertexCount() != 0) {
                m_cacheTexture.setActive(true);
                m_cacheTexture.draw(m_currentStrokeTriangles);
                m_cacheTexture.display();
                m_cacheTexture.setActive(false);
            }

            m_currentStroke.clear();
            m_currentStrokeTriangles.clear();
        });
    }

    static Ptr create(sf::RenderWindow& realWindow, sf::Vector2f size, sf::Color strokeColor, float lineThickness, float spacing) {
        auto canvas = std::make_shared<DrawingCanvas>(&realWindow, strokeColor, lineThickness, spacing);
        canvas->setFocusable(true);
        canvas->setSize(tgui::Layout2d(size));
        canvas->initRenderTexture();
        return canvas;
    }

    void initRenderTexture() {
        m_cacheTexture = sf::RenderTexture({(unsigned int)getSize().x, (unsigned int)getSize().y});
        m_cacheTexture.clear(sf::Color::White);
        m_cacheTexture.display();
    }

    void updateGraphics() {
        if (!m_mouseOnCanvas)
            m_drawing = false;

        if (m_drawing) {
            sf::Vector2i rawMousePos = sf::Mouse::getPosition(*m_realWindow);
            sf::Vector2f currentPos = this->mapPixelToCoords({ (float)(rawMousePos.x - getPosition().x), (float)(rawMousePos.y - getPosition().y) }); // take difference to make the position relative
            float dist = std::hypot(currentPos.x - m_lastPos.x, currentPos.y - m_lastPos.y);
            int steps = std::max(1, static_cast<int>(dist / m_spacing));

            for (int i = 1; i <= steps; ++i) {
                float t = static_cast<float>(i) / steps;
                sf::Vector2f interp = m_lastPos + t * (currentPos - m_lastPos);
                m_currentStroke.push_back(interp);

                if (m_currentStroke.size() >= 2) {
                    auto tri = createThickLine(m_currentStroke[m_currentStroke.size() - 2], interp, m_lineThickness, m_strokeColor);
                    for (auto t : tri){
                        m_currentStrokeTriangles.append(t);
                    }
                }
            }

            m_lastPos = currentPos;
        }

        this->clear();
        this->draw(sf::Sprite(m_cacheTexture.getTexture()));
        this->draw(m_currentStrokeTriangles);
        this->display();
    }

    void clearCanvas(){
        m_cacheTexture.clear(sf::Color::White);
        m_cacheTexture.display();
        m_currentStroke.clear();
        m_currentStrokeTriangles.clear();
    }

    // getter/setter functions
    void setStrokeColor(sf::Color strokeColor){ m_strokeColor = strokeColor; }
    void setLineThickness(float lineThickness){ m_lineThickness = lineThickness; }
    void setSpacing(float spacing){ m_spacing = spacing; }
    sf::Color getStrokeColor(){ return m_strokeColor; }
    float getLineThickness(){ return m_lineThickness; }
    float getSpacing(){ return m_spacing; }

protected:


private:
    sf::RenderWindow* m_realWindow;
    sf::RenderTexture m_cacheTexture;
    sf::Color m_strokeColor;
    float m_lineThickness;
    float m_spacing;

    bool m_mouseOnCanvas = false;
    bool m_drawing = false;
    sf::Vector2f m_lastPos;

    std::vector<sf::Vector2f> m_currentStroke;
    sf::VertexArray m_currentStrokeTriangles{ sf::PrimitiveType::Triangles };
    // Helper to generate a thick line as two triangles
    std::vector<sf::Vertex> createThickLine(const sf::Vector2f& a, const sf::Vector2f& b, float thickness, sf::Color color) {
        sf::Vector2f direction = b - a;
        sf::Vector2f unit = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
        sf::Vector2f normal(-unit.y, unit.x);
        sf::Vector2f offset = normal * (thickness / 2.f);

        std::vector<sf::Vertex> verts(6);
        verts[0] = constructVertex(a - offset, color);
        verts[1] = constructVertex(b - offset, color);
        verts[2] = constructVertex(b + offset, color);
        verts[3] = constructVertex(b + offset, color);
        verts[4] = constructVertex(a + offset, color);
        verts[5] = constructVertex(a - offset, color);
        return verts;
    }
};

int main() {
    const unsigned int windowWidth = 800;
    const unsigned int windowHeight = 600;
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Pizarra", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(120);


    tgui::Gui gui{window};
    auto whiteBoardPanel = tgui::Panel::create({windowWidth, windowHeight - 100});
    auto whiteBoardCanvas = DrawingCanvas::create(window, {windowWidth, windowHeight - 100}, sf::Color::Black, 10.0f, 5.0f);
    whiteBoardPanel->setFocusable(true);
    //whiteBoardCanvas->setFocused(true);
    auto brushPanel = tgui::Panel::create({windowWidth, 100});
    brushPanel->setPosition({0, windowHeight - 100});
    // Set background color via renderer
    brushPanel->getRenderer()->setBackgroundColor(tgui::Color(32, 32, 32)); 

    sf::Color brushColors[] = {sf::Color::Black, sf::Color::White, sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Blue, sf::Color::Magenta};
    CircleButton::Ptr brushButtons[sizeof(brushColors)/sizeof(sf::Color)];
    whiteBoardPanel->add(whiteBoardCanvas);
    gui.add(brushPanel);
    gui.add(whiteBoardPanel);
    if(true){ // keep these out of the function's overall scope, and avoid conflict with the for loop ahead
        auto button = CircleButton::create(20, sf::Color::Black, sf::Color(16, 16, 16));
        button->setPosition({windowWidth - 50*sizeof(brushColors)/sizeof(sf::Color) - 150, (float)windowHeight - 70.f});
        button->getSignal("Clicked").connect([&brushButtons, button, brushColors, &whiteBoardCanvas, &gui](){
            for (int j = 0; j < sizeof(brushColors)/sizeof(sf::Color); j++){
                brushButtons[j]->getSize();
                brushButtons[j]->setOutline(sf::Color::White, 0);
            }
            button->setOutline(sf::Color::White, 3);
            auto colorPicker = tgui::ColorPicker::create("Custom Brush Color");
            colorPicker->onClosing([&whiteBoardCanvas, colorPicker](){
                whiteBoardCanvas->setStrokeColor(colorPicker->getColor());
            });
            colorPicker->onColorChange([&button, colorPicker](){
                button->setNormalColor(colorPicker->getColor());
            });
            gui.add(colorPicker);
        });
        gui.add(button);
    }
    for (int i = 0; i < sizeof(brushColors)/sizeof(sf::Color); i++){
        auto button = CircleButton::create(20, brushColors[i], brushColors[i]);
        button->setPosition({windowWidth - 50*sizeof(brushColors)/sizeof(sf::Color) - 100 + i*50.f, (float)windowHeight - 70.f});
        button->getSignal("Clicked").connect([&brushButtons, i, brushColors, &whiteBoardCanvas](){
            for (int j = 0; j < sizeof(brushColors)/sizeof(sf::Color); j++){
                brushButtons[j]->getSize();
                brushButtons[j]->setOutline(sf::Color::White, 0);
            }
            brushButtons[i]->setOutline(sf::Color::White, 3);
            whiteBoardCanvas->setStrokeColor(brushButtons[i]->getNormalColor());
        });
        brushButtons[i] = button;
        gui.add(button);
    } 
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                window.close();
            if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::C) {
                std::cout << "clear\n";
                whiteBoardCanvas->clearCanvas();
            }
            whiteBoardCanvas->updateGraphics();
        }
        window.clear(sf::Color::Black);
        gui.draw();
        window.display();
    }

    return 0;
}