#pragma once
#include <SFML/Graphics.hpp>
#include <SelbaWard.hpp>
class Mario {
public:
    Mario();
    void move(float deltaX, float deltaY);
    void jump();
    void render(sf::RenderWindow& window);
private:
    sw::GallerySprite sprite;
    sf::Texture spritesheet;
    float x, y;
    float speed;
    bool isJumping;
    float jumpHeight;
    float gravity;
    sf::Clock clock;
};