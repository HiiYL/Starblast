#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <memory>
#include <ctime>
#include <random>
#include <sstream>
using namespace std;
double inline returnModulus(double i)   {
    if(i > 0)
        return i;
    else
        return -i;
}
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
class Bullets : public sf::Drawable, public sf::Transformable{
    friend class Enemy;
public:
    Bullets() {buffer.loadFromFile("fireball.ogg");};
    //void setSpeed(const double &i) {proj_speed = i;};
    void setPowerUp(const int i, const int j) {power_up_type = i; power_up_left = j;};
    bool hasPowerUp() {return (power_up_type!=0);};
    void update(bool &mouse_click,sf::Vector2f origin, sf::Time elapsed);
    void clear() {all_bullets.clear();};
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = NULL;
        for(auto &i : all_bullets)
            target.draw(i->bullet, states);
    }
private:
    struct Bullet{
        Bullet(sf::Vector2f origin, int pow_type)
        {
            powerUpType = pow_type;
            if(pow_type == 1)
                bullet.setSize(sf::Vector2f(150,99));
            else
                bullet.setSize(sf::Vector2f(50,33));
            m_texture.loadFromFile("fireball.png");
            bullet.setTexture(&m_texture);
            bullet.setPosition(origin);
        };
        int powerUpType = 0;
        sf::Texture m_texture;
        sf::RectangleShape bullet;
        double proj_speed = 5;
        sf::Time lifetime;
    };

    int power_up_type = 0;
    int power_up_left = 0;
    sf::SoundBuffer buffer;

    sf::Sound sound;

    //sf::Vector2f velocity;
    vector<shared_ptr<Bullet>> all_bullets;
};
void Bullets::update(bool &mouse_click,sf::Vector2f origin, sf::Time elapsed) {
    if(mouse_click) {

        sound.setBuffer(buffer);
        sound.play();
        cout << power_up_left << endl;
        if(power_up_type != 0)  {
            if(power_up_left != 0)
                power_up_left--;
            else
                power_up_type = 0;
        }
        mouse_click = false;
        shared_ptr<Bullet> new_bullet= make_shared<Bullet>(origin, power_up_type);
        all_bullets.push_back(new_bullet);
    }
    for(auto i = all_bullets.begin(); i !=all_bullets.end();)  {
        if((*i)->bullet.getPosition().x>800)        //if out of screen, cull bullet
            i = all_bullets.erase(i);
        else    {
            //velocity = sf::Vector2f(cos((*i)->angle)*proj_speed,sin((*i)->angle)*proj_speed);
            //(*i)->bullet.move(velocity.x*60*elapsed.asSeconds(),velocity.y*60*elapsed.asSeconds());
            (*i)->bullet.move(((*i)->proj_speed*60*elapsed.asSeconds()),0);
            i++;
        }
    }
}
class Enemy : public sf::Drawable , public sf::Transformable {
 public:
     Enemy();
     bool checkHit(Bullets& bul);
     bool isDead() {return (health<=0);}
     void update(Bullets& bul, sf::Time elapsed);
     void increaseHealth() {health++;};
     void increaseHealth(const int i)   {health += i;};
     void increaseSpeed() {move_speed+=1;};
     void increaseSpeed(const int i) {move_speed += i;};
     void setPowerUp(int i);
     bool isPowerUp() {return (PowerUp);};
     void setSize(int i, int j) {box.setSize(sf::Vector2f(i,j));};
     void setColor(sf::Color color) {box.setFillColor(color);};
     sf::Vector2f getPosition() {return box.getPosition();};
     sf::RectangleShape box;
     int typePowerUp = 0;
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(box);
    }
 private:
     int health = 1;
     bool PowerUp = false;
     sf::Texture m_texture;
     int move_speed = 2;
};
Enemy::Enemy() {
    m_texture.loadFromFile("enemy.png");
    box.setTexture(&m_texture);
    box.setSize(sf::Vector2f(50,50));
    box.setPosition(800,rand()%525);
}
bool Enemy::checkHit(Bullets& bul) {
    for(auto i = bul.all_bullets.begin(); i != bul.all_bullets.end();)
        if(box.getGlobalBounds().intersects((*i)->bullet.getGlobalBounds())) {
            if((*i)->powerUpType == 0)  {
                i = bul.all_bullets.erase(i);
            }
            return true;
        }
        else
            i++;
    return false;
}
void Enemy::update(Bullets& bul, sf::Time elapsed)    {
    if(checkHit(bul))   {
        health--;
    }
    box.move(-move_speed*elapsed.asSeconds()*60,0);
}
void Enemy::setPowerUp(int i) {
    PowerUp = true;
    switch(i)   {
        case 1:
            m_texture.loadFromFile("fireball.png");
            typePowerUp = 1;
            break;
        case 2:
            m_texture.loadFromFile("plus.png");
            typePowerUp = 2;
            break;
    }
    health=9999;
}
int main()
{
    srand (time(0));
    bool only_once = true;
    int score = 0;
    bool is_paused = true;
    Bullets test;
    double player_speed = 5;
    int player_health = 15;
    double angle;
    sf::Vector2f velocity;
    bool mouse_press = false;
    sf::Clock clock;
    sf::Time elapsed;
    vector<shared_ptr<Enemy>> enemies;
    sf::RenderWindow window(sf::VideoMode(800,600),"StarBlast");
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(1);
    sf::RectangleShape bullet;
    bullet.setSize(sf::Vector2f(5,5));
    bullet.setFillColor(sf::Color::Red);
    bullet.setOrigin(bullet.getSize().x/2,bullet.getSize().y/2);
    sf::Text text;
    sf::Font font;
    font.loadFromFile("Simplifica.ttf");
    text.setFont(font);
    text.setString("             Press Spacebar to Begin \n Click to Shoot, Move Cursor to Move(duh) ");
    text.setPosition((window.getSize().x-text.getGlobalBounds().width)/2,
                     (window.getSize().y-text.getGlobalBounds().height)/2);
    sf::Text score_text;
    score_text.setFont(font);
    score_text.setString("SCORE : 0");
    sf::Text health;
    health.setFont(font);
    health.setString("HEALTH : 15");
    health.setPosition(window.getSize().x-100,0);
    sf::Clock score_clock;
    sf::Time score_elapsed;
    sf::Texture player_texture;
    sf::Image image;
    sf::Clock AnimationClock;
    sf::Time AnimationElapsed;
    int currFrame = 0;
    sf::Sprite player;

    //player.setSize(sf::Vector2f(100,100));
    player.setPosition(50,25);

    image.loadFromFile("okami.png");
    sf::IntRect r1(0,0,131,85);
    player_texture.loadFromImage(image,r1);
    player.setTexture(player_texture);
    player.setOrigin(player.getGlobalBounds().width/2,player.getGlobalBounds().height/2);


    int frameX = 0, frameY = 0;
    sf::Sprite background;
    sf::Texture background_texture;
    background_texture.loadFromFile("background.png");
    background.setTexture(background_texture);
    sf::Clock difficultyTimer;
    sf::Time difficultyElapsed;
    sf::Text difficultyText;

    difficultyText.setFont(font);
    difficultyText.setString("LEVEL :0");
    difficultyText.setPosition((window.getSize().x-difficultyText.getGlobalBounds().width)/2,0);
    int currDifficulty = 0;
    sf::Clock gameLogic;
    sf::Time gameLogicElapsed;
    double difficultyElapsedTime = 0;
    int counter = 0;
    double powerUpTimer = 0;
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::MouseButtonReleased)
                mouse_press = true;
            if(event.type == sf::Event::KeyPressed)    {
                if(event.key.code == sf::Keyboard::Space)   {
                    if(player_health<=0)    {
                        player_health = 15;
                        score = 0;
                        currDifficulty = 0;
                        enemies.clear();
                        test.clear();
                        difficultyText.setString("LEVEL : 0");
                        difficultyTimer.restart();
                    }
                    is_paused = !is_paused;
                }
                if(event.key.code == sf::Keyboard::Delete)  {
                    enemies.clear();
                }
            }
        }
        AnimationElapsed = AnimationClock.getElapsedTime();
        powerUpTimer+=gameLogicElapsed.asSeconds();
        gameLogicElapsed = gameLogic.restart();
        if(AnimationElapsed.asMilliseconds()>33)   {
            if(currFrame>7) {
                currFrame = 0;
                frameY = 0;
                frameX = 0;
            }
            AnimationClock.restart();
            if(!(currFrame%2))  {
                    if(currFrame!=0)    {
                        if(currFrame<7) {
                            frameY=0;
                            frameX++;
                        }
                        else    {
                            frameY=3;
                            frameX++;
                        }

                    }
            }
            if(currFrame == 6)  {
                frameY=2;
                frameX=0;
            }
            sf::IntRect r2((frameX)*131,(frameY)*85,131,85);
            player_texture.loadFromImage(image,r2);
            frameY++;
            currFrame++;
        }
        score_text.setString("SCORE : " + patch::to_string(score));
        health.setString("HEALTH : " + patch::to_string(player_health));
        if(!is_paused)  {
            difficultyElapsed = difficultyTimer.restart();

            text.setString("");
            elapsed = clock.getElapsedTime();
            score_elapsed = score_clock.getElapsedTime();
            sf::Vector2f mouseloc = sf::Vector2f(sf::Mouse::getPosition(window));
                //double result = atan2(mouseloc.y-player.getPosition().y,mouseloc.x-player.getPosition().x);
                //if(result < 0)  {
                //    result = 2*PI+result;
                //}
                //RIP MOVEMMENT ALGORITHM, YOU WILL BE MISSED
                //player.setPosition(player.getPosition().x,mouseloc.y);
            if(!((mouseloc.y<75 || mouseloc.y>525)&&(player.getPosition().y<75||player.getPosition().y>525))) //prevent user from hiding outside screen
                player.move(0, (mouseloc.y-player.getPosition().y)/player_speed);
            if(!((mouseloc.x<75 || mouseloc.x>725)&&(player.getPosition().x<75||player.getPosition().x>725))) //prevent user from hiding outside screen
                player.move(((mouseloc.x-player.getPosition().x)/player_speed),0) ;
                //if(returnModulus(mouseloc.x-player.getPosition().x) < player_speed) {
                //    player.move(mouseloc.x-player.getPosition().x,0);
                //}
                //else
                //    player.move(cos(result)*player_speed,0);
            if(score_elapsed.asSeconds()>1) {
                score+=5;
                score_clock.restart();
            }
            difficultyElapsedTime+=difficultyElapsed.asSeconds();
            if(difficultyElapsedTime>15)    {
                        difficultyText.setString("LEVEL : " + patch::to_string(++currDifficulty));
                        difficultyElapsedTime = 0;
            }
            if(elapsed.asSeconds()>(0.5/(0.5*currDifficulty+1)))   {
                shared_ptr<Enemy> currEnemy = make_shared<Enemy>();

                if(powerUpTimer > 15)    {
                    powerUpTimer = 0;
                    currEnemy->setPowerUp(rand()%2+1);
                }
                else
                {
                    currEnemy->increaseHealth(currDifficulty);
                    currEnemy->increaseSpeed(currDifficulty);
                    switch(currDifficulty)  {
                        case 0:
                            break;
                        case 1:
                            currEnemy->setColor(sf::Color::Green);
                            break;
                        case 2:
                            currEnemy->setColor(sf::Color::Blue);
                            break;
                        case 3:
                            currEnemy->setColor(sf::Color::Black);
                            break;
                        default:
                            currEnemy->setColor(sf::Color::Cyan);
                            break;
                    }

                }
                enemies.push_back(currEnemy);
                clock.restart();
            }
            //angle = atan2(mouseloc.y-player.getPosition().y,mouseloc.x-player.getPosition().x);
            window.clear(sf::Color::Black);
            window.draw(background);
            window.draw(player);
            for(auto i = enemies.begin();i!=enemies.end();)  {
                (*i)->update(test,gameLogicElapsed);
                window.draw(**i);
                if((player.getGlobalBounds().intersects((*i)->box.getGlobalBounds())) || (*i)->isDead() ||
                    (*i)->getPosition().x<-50)   {       //culls enemies once no longer visible
                   if((player.getGlobalBounds().intersects((*i)->box.getGlobalBounds()))&&(!(*i)->isPowerUp()))
                        --player_health;
                    if((*i)->isPowerUp())   {
                        if((*i)->typePowerUp == 1)
                            test.setPowerUp(1,15);
                        else
                            player_health+=5;

                    }
                   if((*i)->isDead())
                      score+=10;
                    i = enemies.erase(i);
                }

                else
                    i++;
            }
            test.update(mouse_press,player.getPosition(), gameLogicElapsed);
            window.draw(test);
            if(player_health <= 0)
                is_paused = true;
        }
        else
        {
            window.clear(sf::Color(0,0,0,127));
            if(player_health<=0) {
                text.setString("         Game Over. Press Spacebar to Play Again");
            }
            else
            text.setString("             Press Spacebar to Begin \n Click to Shoot, Move Cursor to Move(duh) ");
        }


        window.draw(text);
        window.draw(difficultyText);
        window.draw(score_text);
        window.draw(health);
        window.display();
    }
}
