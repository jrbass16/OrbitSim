#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

const float PI = 3.14159265;
const float G = 6.67430e-11;
const int MAX_FRAMERATE = 30;
//const float DT = 86400; //can set as a constant, but I want it proportional to the period of the orbit
const std::size_t MAX_PATH_SIZE = 100;
const float SCALE = 10e9;
const unsigned int WINDOW_HEIGHT = 1080;
const unsigned int WINDOW_WIDTH = 1920;

class Body{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Vector2f displayPos;
    float mass;

    sf::CircleShape s;
public:

    Body(float pos_x, float pos_y, float vel_x, float vel_y, float mass)
    {
      
        pos.x = pos_x * SCALE;
        pos.y = pos_y * SCALE;

        vel.x = vel_x;
        vel.y = vel_y;
      
        this->mass = mass;

        s.setPosition(pos);
        s.setFillColor(sf::Color::White);
        s.setRadius(8);
      
        displayPos.x = pos_x;
        displayPos.y = pos_y;
    }

    void updatePhysics(Body &b, float dt){
        float distance_x = b.getPos().x - pos.x;
        float distance_y = b.getPos().y - pos.y;
        //radius (distance) between the sc and body
        float radius = sqrt(distance_x*distance_x + distance_y*distance_y);  
        //constant of otherMass/r^3
        float k = b.getMass() / (radius*radius*radius);
        //finding x and y componant
        float accel_x = G * distance_x * k;
        float accel_y = G * distance_y * k;

        vel.x += accel_x*dt;
        vel.y += accel_y*dt;

        pos.x += vel.x*dt;
        pos.y += vel.y*dt;
      
        displayPos.x = pos.x/SCALE;
        displayPos.y = pos.y/SCALE;
    }

    void render(sf::RenderWindow &wind){
        s.setPosition(displayPos);
        wind.draw(s);
    }
   
    sf::Vector2f getPos(){return pos;}
    sf::Vector2f getDispPos(){return displayPos;}
    float getMass(){return mass;}
};

int main()
{
    auto window = sf::RenderWindow{ {WINDOW_WIDTH, WINDOW_HEIGHT}, "Orbit Simulator" };
    window.setFramerateLimit(MAX_FRAMERATE);
    //Initialize all of our objects
    Body sun(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0, 0, 1.989e30);

    //Calculate some orbital constants for the earth
    float semiMajorAxis = 150*SCALE;
    float eccen = 0.0;
    float initAngle = 0.0167;
    float initRadius = semiMajorAxis * (1.0 - eccen);
    sf::Vector2f initPos(sun.getPos().x + initRadius, sun.getPos().y);
    float initVel = std::sqrt(G * sun.getMass() / initRadius);
    //Calculate period, then use that to find dt so a full rotation takes 30 seconds
    float period = 2*PI*std::sqrt(semiMajorAxis*semiMajorAxis*semiMajorAxis / (G * sun.getMass()));
    std::cout<<period<<std::endl;
    float dt_per_second = period/10;
    std::cout<<dt_per_second<<std::endl;
    //Initialize
    Body earth(initPos.x/SCALE, initPos.y/SCALE, 0, initVel, 5.972e24);
    //Create variables to track time and path
    std::vector<sf::Vector2f> planetPath;
    
    //paused flag
    bool paused = true;
    sf::Clock clock;

	while(window.isOpen())
    {
		sf::Event event;
		while (window.pollEvent(event))
        {//look for events at beginning of loop and perform actions based on that
			if (event.type == sf::Event::Closed) window.close();
            else if(event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space) paused = !paused;
            }
		}
    //keeps movement consistant with real time even if framerate shutters
    float elapsedTime = clock.restart().asSeconds();
    float dt = elapsedTime*dt_per_second;
    //take the amount of dt we are supposed to have every second and multiply by the actual amount fo second that has passed this frame
   

    if (!paused){

        planetPath.push_back(sf::Vector2f(earth.getDispPos().x, earth.getDispPos().y));
	
        if (planetPath.size() >= MAX_PATH_SIZE)
        {
            planetPath.erase(planetPath.begin()); 
        }
        earth.updatePhysics(sun, dt);
    }

    window.clear();
    //debug
    //std::cout<<"Earth Pos:\t(" << earth.getPos().x <<", "<<earth.getPos().y<<")"<<std::endl;
    //std::cout<<"Display Pos:\t(" << earth.getDispPos().x<<", "<<earth.getDispPos().y<<")"<<std::endl;
    sun.render(window);
    earth.render(window);

    sf::VertexArray path(sf::LineStrip, planetPath.size());
    for (std::size_t i = 0; i < planetPath.size(); ++i)
    {
        path[i].position.x = planetPath[i].x + 8;
        path[i].position.y = planetPath[i].y + 8;
        path[i].color = sf::Color(0,255,0,255 - (int)i);
    }
    window.draw(path);
    //Final display
	window.display();
	}
return 0;
}