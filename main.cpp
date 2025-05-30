#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SelbaWard.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "whiteboard.hpp"
#include "Engine/Engine.hpp"

#define TILESIZE 32

std::unique_ptr<sf::RenderTexture> createGrid(sf::RenderWindow& window){
    std::unique_ptr<sf::RenderTexture> grid(new sf::RenderTexture(window.getSize()));
    int windowWidth = window.getSize().x;
    int windowHeight = window.getSize().y;
    grid->clear(sf::Color::Transparent);
    for (int i = 0; i < 24; i++){
        sw::Line gridLine({(float)(i*TILESIZE - 1), 0}, {(float)(i*TILESIZE - 1), (float)windowHeight}, 2, sf::Color::Black);
        grid->draw(gridLine);
    }
    for (int i = 0; i < 14; i++){
        sw::Line gridLine({0, (float)(i*TILESIZE - 1)}, {(float)windowWidth, (float)(i*TILESIZE - 1)}, 2, sf::Color::Black);
        grid->draw(gridLine);
    }
    grid->display();
    return grid;
}

int main(){
    bool gridEnabled = true;

    sf::Color skyColor(147, 187, 236);
    
    const unsigned int windowWidth = TILESIZE * 24;
    const unsigned int windowHeight = TILESIZE * 14;
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode({windowWidth, windowHeight}), "Pizarra", sf::Style::Default, sf::State::Windowed, settings);
    window.setFramerateLimit(120);
    
    auto gridTexture = createGrid(window);
    sf::Sprite grid(gridTexture->getTexture());

    tgui::Gui gui{window};

    Mario mario;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(skyColor);
        if (gridEnabled){
            window.draw(grid);
        }
        mario.render(window);
        gui.draw();
        window.display();
    }
    openWhiteboardWindow();
    return 0;
}