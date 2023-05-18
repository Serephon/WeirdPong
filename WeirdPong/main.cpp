#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "Collision.h"
#include <cstdlib>
#include <time.h>
#include <cmath>

int main(int, char const**)
{
    //Main Window
    sf::RenderWindow app(sf::VideoMode(800, 600), "SFML window");
    app.setFramerateLimit(60);
    srand (time(NULL));

    //Sounds
    sf::SoundBuffer bufferRacketHit;
    sf::SoundBuffer bufferWallHit;
    sf::SoundBuffer bufferScore;
    if(!bufferRacketHit.loadFromFile("ping_pong_8bit_racket.wav") || !bufferWallHit.loadFromFile("ping_pong_8bit_wall.wav") || !bufferScore.loadFromFile("ping_pong_8bit_score.wav")){
        return -1;
    }
    sf::Sound soundRacketHit;
    soundRacketHit.setBuffer(bufferRacketHit);
    sf::Sound soundWallHit;
    soundWallHit.setBuffer(bufferWallHit);
    sf::Sound soundScore;
    soundScore.setBuffer(bufferScore);

    //Font
    sf::Font font;
    font.loadFromFile("VCR_OSD_MONO_1.001.ttf");

    //Scoreboard
    sf::Text scoreLeft;
    scoreLeft.setFont(font);
    scoreLeft.setOrigin(scoreLeft.getLocalBounds().width / 2, scoreLeft.getLocalBounds().height / 2);
    scoreLeft.setPosition(300, 80);
    scoreLeft.setCharacterSize(48);
    scoreLeft.setFillColor(sf::Color::White);
    scoreLeft.setStyle(sf::Text::Bold);
    int scoreValueLeft = 0;

    sf::Text scoreRight;
    scoreRight.setFont(font);
    scoreRight.setOrigin(scoreRight.getLocalBounds().width / 2, scoreRight.getLocalBounds().height / 2);
    scoreRight.setPosition(460, 80);
    scoreRight.setCharacterSize(48);
    scoreRight.setFillColor(sf::Color::White);
    scoreRight.setStyle(sf::Text::Bold);
    int scoreValueRight = 0;

    //Camera Rotation Text display
    sf::Text cameraText;
    cameraText.setFont(font);
    cameraText.setOrigin(cameraText.getLocalBounds().width / 2, cameraText.getLocalBounds().height / 2);
    cameraText.setPosition(70, 70);
    cameraText.setCharacterSize(16);
    cameraText.setFillColor(sf::Color::White);
    cameraText.setStyle(sf::Text::Bold);
    cameraText.setString("Press 'A, D, Left, Right'\nto rotate camera");

    // Load sprites to display
    sf::Texture textureBall;
    sf::Texture textureDottedLine;
    sf::Texture textureRacketLeft;
    sf::Texture textureRacketRight;
    sf::Texture textureWallUp;
    sf::Texture textureWallDown;
    sf::Texture textureWallLeft;
    sf::Texture textureWallRight;
    if (!textureBall.loadFromFile("Ball.png") || !textureDottedLine.loadFromFile("DottedLine.png") || !textureRacketLeft.loadFromFile("Racket.png") ||
        !textureRacketRight.loadFromFile("Racket.png") || !textureWallUp.loadFromFile("WallHorizontal.png") || !textureWallLeft.loadFromFile("WallVertical.png") ||
        !textureWallDown.loadFromFile("WallHorizontal.png") || !textureWallRight.loadFromFile("WallVertical.png"))
        return EXIT_FAILURE;
    sf::Sprite spriteBall(textureBall);
    sf::Sprite spriteDottedLine(textureDottedLine);
    sf::Sprite spriteRacketLeft(textureRacketLeft);
    sf::Sprite spriteRacketRight(textureRacketRight);
    sf::Sprite spriteWallUp(textureWallUp);
    sf::Sprite spriteWallLeft(textureWallLeft);
    sf::Sprite spriteWallDown(textureWallDown);
    sf::Sprite spriteWallRight(textureWallRight);

    //Set Origin, Scale, Position
    spriteBall.setOrigin(spriteBall.getLocalBounds().height / 2, spriteBall.getLocalBounds().width / 2);
    spriteDottedLine.setOrigin(spriteDottedLine.getLocalBounds().height / 2, spriteDottedLine.getLocalBounds().width / 2);
    spriteRacketLeft.setOrigin(spriteRacketLeft.getLocalBounds().height / 2, spriteRacketLeft.getLocalBounds().width / 2);
    spriteRacketRight.setOrigin(spriteRacketRight.getLocalBounds().height / 2, spriteRacketRight.getLocalBounds().width / 2);
    spriteWallUp.setOrigin(spriteWallUp.getLocalBounds().height / 2, spriteWallUp.getLocalBounds().width / 2);
    spriteWallLeft.setOrigin(spriteWallLeft.getLocalBounds().height / 2, spriteWallLeft.getLocalBounds().width / 2);
    spriteWallDown.setOrigin(spriteWallDown.getLocalBounds().height / 2, spriteWallDown.getLocalBounds().width / 2);
    spriteWallRight.setOrigin(spriteWallRight.getLocalBounds().height / 2, spriteWallRight.getLocalBounds().width / 2);

    spriteBall.setPosition(400, 300);
    spriteDottedLine.setPosition(620, 80);
    spriteRacketLeft.setPosition(100, 300);
    spriteRacketRight.setPosition(715, 300);
    spriteWallUp.setPosition(50, 300);
    spriteWallLeft.setPosition(200, 58);
    spriteWallDown.setPosition(50, 802);
    spriteWallRight.setPosition(900, 58);

    spriteBall.setScale(12.f, 12.f);
    spriteDottedLine.setScale(16.f, 16.f);
    spriteRacketLeft.setScale(10.f, 16.f);
    spriteRacketRight.setScale(10.f, 16.f);
    spriteWallUp.setScale(14.f, 10.f);
    spriteWallLeft.setScale(10.f, 16.f);
    spriteWallDown.setScale(14.f, 10.f);
    spriteWallRight.setScale(10.f, 16.f);

    //Camera View
    sf::View cameraView(sf::FloatRect(0.f, 0.f, app.getSize().x, app.getSize().y));
    cameraView.setViewport(sf::FloatRect(0.f, 0.f, 1.0f, 1.0f));

    float racketSpeed = 10.0f;
    float ballSpeed = 5.0f;
    int directionX = 1 * ballSpeed;
    int directionY = 1 * ballSpeed;

	// Start the game loop
    while (app.isOpen())
    {
        // Process events
        sf::Event event;
        while (app.pollEvent(event))
        {
            // Close window : exit
            if((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
                app.close();
        }

        //Racket movement
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            spriteRacketLeft.move(sf::Vector2f(0.f, -1.0f * racketSpeed));
            cameraView.rotate(0.5f);
        }if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            spriteRacketLeft.move(sf::Vector2f(0.f, 1.0f * racketSpeed));
            cameraView.rotate(-0.5f);
        }if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            spriteRacketRight.move(sf::Vector2f(0.f, -1.0f * racketSpeed));
            cameraView.rotate(0.5f);
        }if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            spriteRacketRight.move(sf::Vector2f(0.f, 1.0f * racketSpeed));
            cameraView.rotate(-0.5f);
        }

        //Ball movement
        spriteBall.move(directionX, directionY);

        //Collision check Up % Down Walls
        if(Collision::BoundingBoxTest(spriteBall, spriteWallDown)){
            directionY = -1 * ballSpeed;
            soundWallHit.play();
        }
        if(Collision::BoundingBoxTest(spriteBall, spriteWallUp)){
            directionY = 1 * ballSpeed;
            soundWallHit.play();
        }

        //Collision check Left & Right Walls
        if(Collision::BoundingBoxTest(spriteBall,spriteWallLeft)){
            scoreValueRight = scoreValueRight + 1;
            spriteBall.setPosition(400 ,300);
            ballSpeed = 5.0f;
            soundScore.play();
        }
        if(Collision::BoundingBoxTest(spriteBall,spriteWallRight)){
            scoreValueLeft = scoreValueLeft + 1;
            spriteBall.setPosition(400 ,300);
            ballSpeed = 5.0f;
            soundScore.play();
        }

        //Collision check Rackets
        if(Collision::BoundingBoxTest(spriteBall, spriteRacketLeft)){
            directionX = 1 * ballSpeed;
            ballSpeed++;
            soundRacketHit.play();
        }
        if(Collision::BoundingBoxTest(spriteBall, spriteRacketRight)){
            directionX = -1 * ballSpeed;
            ballSpeed++;
            soundRacketHit.play();
        }

        //Collision check Rackets with Walls
        if(Collision::BoundingBoxTest(spriteRacketLeft, spriteWallUp)){
            spriteRacketLeft.move(sf::Vector2f(0.f, 1.f * racketSpeed));
        }
        if(Collision::BoundingBoxTest(spriteRacketLeft, spriteWallDown)){
            spriteRacketLeft.move(sf::Vector2f(0.f, -1.f * racketSpeed));
        }
        if(Collision::BoundingBoxTest(spriteRacketRight, spriteWallUp)){
            spriteRacketRight.move(sf::Vector2f(0.f, 1.f * racketSpeed));
        }
        if(Collision::BoundingBoxTest(spriteRacketRight, spriteWallDown)){
            spriteRacketRight.move(sf::Vector2f(0.f, -1.f * racketSpeed));
        }

        //Camera Rotate
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            cameraView.rotate(0.5f);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            cameraView.rotate(-0.5f);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            cameraView.rotate(0.5f);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            cameraView.rotate(-0.5f);
        }

        //Scoreboard Update
        std::stringstream ss1;
        ss1 << scoreValueLeft;
        scoreLeft.setString(ss1.str());
        std::stringstream ss2;
        ss2 << scoreValueRight;
        scoreRight.setString(ss2.str());

        // Clear screen
        app.clear();

        // Draw the sprite
        app.setView(cameraView);
        app.draw(spriteBall);
        app.draw(spriteDottedLine);
        app.draw(spriteRacketLeft);
        app.draw(spriteRacketRight);
        app.draw(spriteWallUp);
        app.draw(spriteWallLeft);
        app.draw(spriteWallDown);
        app.draw(spriteWallRight);
        app.draw(scoreLeft);
        app.draw(scoreRight);
        app.draw(cameraText);

        // Update the window
        app.display();
    }

    return EXIT_SUCCESS;
}
