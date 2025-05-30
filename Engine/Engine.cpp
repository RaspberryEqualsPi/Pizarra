#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SelbaWard.hpp>
#include <iostream>
#include <vector>
#include "../gfx/mario.h"

#include "Engine.hpp"

Mario::Mario(){
    
    spritesheet.loadFromMemory(mario_png, sizeof(mario_png));
    sprite.setTexture(spritesheet);
    
    // adds exhibits from spritesheet for each state
    sprite.addExhibit(sf::FloatRect({9, 22}, {16,16}));
    sprite.addExhibit(sf::FloatRect({46, 22}, {16,16}));
        
    for (int i = 0; i < 3; i++){
        sprite.addExhibit(sf::FloatRect({83 + 33 * (float)i, 22}, {16,16}));
    }
    sprite.addExhibit(sf::FloatRect({186, 22}, {16,16})); // one here
    for (int i = 0; i < 2; i++){
        sprite.addExhibit(sf::FloatRect({223 + 33 * (float)i, 22}, {16,16}));
    }
    for (int i = 0; i < 2; i++){
        sprite.addExhibit(sf::FloatRect({293 + 33 * (float)i, 22}, {16,16}));
    }
    for (int i = 0; i < 6; i++){
        sprite.addExhibit(sf::FloatRect({363 + 33 * (float)i, 22}, {16,16}));
    }
    for (int i = 0; i < 2; i++){
        sprite.addExhibit(sf::FloatRect({565 + 33 * (float)i, 22}, {16,16}));
    }
    sprite.addExhibit(sf::FloatRect({635, 22}, {16,16})); // one here
    for (int i = 0; i < 2; i++){
        sprite.addExhibit(sf::FloatRect({672 + 33 * (float)i, 22}, {16,16}));
    }
    for (int i = 0; i < 2; i++){
        sprite.addExhibit(sf::FloatRect({742 + 33 * (float)i, 22}, {16,16}));
    }
    sprite.set(1);
    sprite.scale({2, 2}); //scales 16x16 to 32x32
    
}
void Mario::render(sf::RenderWindow& window){
    sprite.set(clock.getElapsedTime().asMilliseconds() / 250 % 23 + 1); // sets exhibit based on clock, which cycles through exhibits
    window.draw(sprite);
}
