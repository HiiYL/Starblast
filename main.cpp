#include <SFML/Graphics.hpp>
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
    Bullets() {m_texture.loadFromFile("fireball.png");};
    void setSpeed(const double &i) {proj_speed = i;};
    void update(bool &mouse_click,sf::Vector2f origin,const double angle);
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
        Bullet(sf::Vector2f origin,double in_angle, sf::Texture &texture)
        {
            bullet.setSize(sf::Vector2f(50,33));;
            bullet.setTexture(&texture);
            bullet.setPosition(origin);
            angle = in_angle;
        };
        sf::RectangleShape bullet;
        double angle;
        sf::Time lifetime;
    };
    sf::Texture m_texture;
    double proj_speed = 5;
    sf::Vector2f velocity;
    vector<shared_ptr<Bullet>> all_bullets;
};
void Bullets::update(bool &mouse_click,sf::Vector2f origin,const double angle) {

    if(mouse_click) {
        mouse_click = false;
        shared_ptr<Bullet> new_bullet= make_shared<Bullet>(origin,angle,m_texture);
        all_bullets.push_back(new_bullet);
    }
    for(auto &i : all_bullets)  {
        velocity = sf::Vector2f(cos(i->angle)*proj_speed,sin(i->angle)*proj_speed);
        i->bullet.move(velocity.x,velocity.y);
    }
}
class Enemy : public sf::Drawable , public sf::Transformable {
 public:
     Enemy();
     bool checkHit(Bullets& bul);
     bool isDead() {return (health<=0);}
     void update(Bullets& bul);
     sf::RectangleShape box;
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(box);
    }
 private:
     int health = 1;
     sf::Texture m_texture;

};
Enemy::Enemy() {
    m_texture.loadFromFile("enemy.png");
    box.setTexture(&m_texture);
    box.setSize(sf::Vector2f(50,50));
    //box.setFillColor(sf::Color::);
    box.setPosition(800,rand()%600);
}
bool Enemy::checkHit(Bullets& bul) {
    for(auto i = bul.all_bullets.begin(); i != bul.all_bullets.end();)
        if(box.getGlobalBounds().contains((*i)->bullet.getPosition())) {

            i = bul.all_bullets.erase(i);

            return true;
        }
        else
            i++;
    return false;
}
void Enemy::update(Bullets& bul)    {
    if(checkHit(bul))   {
        health--;
    }
    box.move(-2,0);
}
int main()
{
    int score = 0;
    bool is_paused = true;
    Bullets test;
    const double PI = 3.14159265;
    double player_speed = 5;
    int player_health = 15;
    double angle;
    sf::Vector2f velocity;
    bool mouse_press = false;
    sf::Clock clock;
    sf::Time elapsed;
    vector<shared_ptr<Enemy>> enemies;
    sf::RenderWindow window(sf::VideoMode(800,600),"Hello There");
    window.setVerticalSyncEnabled(1);
    sf::RectangleShape player;
    player.setSize(sf::Vector2f(100,100));
    player.setPosition(50,25);
    player.setOrigin(player.getSize().x/2,player.getSize().y/2);
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
    image.loadFromFile("okami.png");
    sf::IntRect r1(0,0,131,85);
    player_texture.loadFromImage(image,r1);
    player.setTexture(&player_texture);
    int frameX = 0, frameY = 0;
    sf::Sprite background;
    sf::Texture background_texture;
    background_texture.loadFromFile("background.png");
    background.setTexture(background_texture);
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::MouseButtonReleased)
                mouse_press = true;
            if(event.type == sf::Event::KeyPressed)
                if(event.key.code == sf::Keyboard::Space)   {
                    if(player_health<=0)    {
                        player_health = 15;
                        score = 0;
                        enemies.clear();
                        test.clear();
                    }
                    is_paused = !is_paused;
                }
        }
        AnimationElapsed = AnimationClock.getElapsedTime();
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
            text.setString("");
            elapsed = clock.getElapsedTime();
            score_elapsed = score_clock.getElapsedTime();
            sf::Vector2f mouseloc = sf::Vector2f(sf::Mouse::getPosition(window));
            if(mouseloc!=player.getPosition())  {
                double result = atan2(mouseloc.y-player.getPosition().y,mouseloc.x-player.getPosition().x);
                if(result < 0)  {
                    result = 2*PI+result;
                }
                if(returnModulus(mouseloc.y-player.getPosition().y) < player_speed) {
                   player.move(0,mouseloc.y-player.getPosition().y);
                }
                else
                    player.move(0,sin(result)*player_speed);
                //if(returnModulus(mouseloc.x-player.getPosition().x) < player_speed) {
                //    player.move(mouseloc.x-player.getPosition().x,0);
                //}
                //else
                //    player.move(cos(result)*player_speed,0);
            }
            if(elapsed.asSeconds()>0.1)   {
                enemies.push_back(make_shared<Enemy>());
                clock.restart();
            }
            if(score_elapsed.asSeconds()>1) {
                score+=5;

                score_clock.restart();
            }
            angle = atan2(mouseloc.y-player.getPosition().y,mouseloc.x-player.getPosition().x);
            test.update(mouse_press,player.getPosition(),angle);
            window.clear(sf::Color::Black);
            window.draw(background);
            window.draw(player);

            for(auto i = enemies.begin();i!=enemies.end();)  {
                (*i)->update(test);
                window.draw(**i);
                if((player.getGlobalBounds().intersects((*i)->box.getGlobalBounds())) || (*i)->isDead())   {
                   if(player.getGlobalBounds().intersects((*i)->box.getGlobalBounds()))
                        --player_health;
                   if((*i)->isDead())
                      score+=10;
                   i = enemies.erase(i);
                }
                else
                    i++;
            }
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

        window.draw(test);
        window.draw(text);
        window.draw(score_text);
        window.draw(health);
        window.display();
    }
}
