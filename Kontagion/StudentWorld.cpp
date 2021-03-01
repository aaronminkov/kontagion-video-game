#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

bool checkDistance(list<Actor*> actors, int x, int y) {
    for (list<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        if ((*it)->blocksBacteriumMovement()) continue;
        double distance = sqrt(pow(((*it)->getX() * 1.0) - (x * 1.0), 2) + pow(((*it)->getY() * 1.0) - (y * 1.0), 2));
        if (distance <= SPRITE_WIDTH)
            return false;
    }
    return true;
}

int StudentWorld::init()
{
    m_socrates = new Socrates(this, 0, VIEW_HEIGHT/2, 0, 0);
    for (int i = 0; i < getLevel(); i++) {
        while (true) {
            int x = randInt(0, VIEW_HEIGHT);
            int y = randInt(0, VIEW_WIDTH);
            double z = pow((x-VIEW_HEIGHT/2), 2) + pow((y-VIEW_WIDTH/2), 2);
            if (z < pow(120, 2) && checkDistance(m_actors, x, y)) {
                m_actors.push_back(new Pit(this, x, y));
                break;
            }
        }
    }
    for (int i = 0; i < min(5 * getLevel(), 25); i++) {
        while (true) {
            int x = randInt(0, VIEW_HEIGHT);
            int y = randInt(0, VIEW_WIDTH);
            double z = pow((x-VIEW_HEIGHT/2), 2) + pow((y-VIEW_WIDTH/2), 2);
            if (z < pow(120, 2) && checkDistance(m_actors, x, y)) {
                m_actors.push_back(new Food(this, x, y));
                break;
            }
        }
    }
    for (int i = 0; i < max(180 - 20 * getLevel(), 20); i++) {
        while (true) {
            int x = randInt(0, VIEW_HEIGHT);
            int y = randInt(0, VIEW_WIDTH);
            double z = pow((x-VIEW_HEIGHT/2), 2) + pow((y-VIEW_WIDTH/2), 2);
            if (z < pow(120, 2) && checkDistance(m_actors, x, y)) {
                m_actors.push_back(new Dirt(this, x, y));
                break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_socrates->getHealth() <= 0) {
        decLives();
        
        playSound(SOUND_PLAYER_DIE);
        return GWSTATUS_PLAYER_DIED;
    }
    m_socrates->doSomething();
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end()) {
        if ((*it)->alive())
        {
            (*it)->doSomething();
            if (m_socrates->getHealth() <= 0) {
                decLives();
                
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
            it++;
        }
        else
        {
            list<Actor*>::iterator deleteMe = it;
            it = m_actors.erase(it);
            delete (*deleteMe);
        }
    }
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if (!(*it)->alive())
        {
            list<Actor*>::iterator deleteMe = it;
            it = m_actors.erase(it);
            delete (*deleteMe);
        }
        else it++;
    }
    it = m_actors.begin();
    bool levelOver = true;
    while (it != m_actors.end())
    {
        if ((*it)->preventsLevelCompleting()) levelOver = false;
        it++;
    }
    if (levelOver) return GWSTATUS_FINISHED_LEVEL;
    
    int chanceFungus = max(510 - getLevel() * 10, 200);
    if (randInt(0, chanceFungus) == 0)
    {
        int dir = PI/180 * randInt(0, 359);
        int newX = VIEW_WIDTH/2 + VIEW_RADIUS * cos(dir);
        int newY = VIEW_HEIGHT/2 + VIEW_RADIUS * sin(dir);
        m_actors.push_back(new Fungus(this, newX, newY));
    }
    
    int chanceGoodie = max(510 - getLevel() * 10, 200);
    if (randInt(0, chanceGoodie) == 0)
    {
        int dir = PI/180 * randInt(0, 359);
        int newX = VIEW_WIDTH/2 + VIEW_RADIUS * cos(dir);
        int newY = VIEW_HEIGHT/2 + VIEW_RADIUS * sin(dir);
        int rand = randInt(1, 10);
        if (rand <= 6)
            m_actors.push_back(new RestoreHealthGoodie(this, newX, newY));
        else if (rand <= 9)
            m_actors.push_back(new FlamethrowerGoodie(this, newX, newY));
        else
            m_actors.push_back(new ExtraLifeGoodie(this, newX, newY));
    }
    
    setStatus();
    
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setStatus() {
    ostringstream status;
    status.fill('0');
    if (getScore() < 0)
        status << "-" <<  std::setw(5) << abs(getScore()) << "  ";
    else
        status << std::setw(5) << getScore() << "  ";
    status.setf(ios::fixed);
    status << "Level: " << getLevel() << "  ";
    status << "Lives: " << getLives() << "  ";
    status << "Health: " << m_socrates->getHealth() << "  ";
    status << "Sprays: " << m_socrates->getSprays() << "  ";
    status << "Flames: " << m_socrates->getFlames();
    std::string text = "Score: " + status.str();
    setGameStatText(text);
}

void StudentWorld::cleanUp()
{
    if (m_socrates != nullptr)
    {
        delete m_socrates;
        m_socrates = nullptr;
    }
    list<Actor*>::iterator it = m_actors.begin();
    while(it != m_actors.end()) {
        list<Actor*>::iterator deleteMe = it;
        it = m_actors.erase(it);
        delete (*deleteMe);
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::createSalmonella(int x, int y)
{
    m_actors.push_back(new Salmonella(this, x, y));
}

void StudentWorld::createAggSalmonella(int x, int y)
{
    m_actors.push_back(new AggressiveSalmonella(this, x, y));
}
void StudentWorld::createEColi(int x, int y)
{
    m_actors.push_back(new EColi(this, x, y));
}

int StudentWorld::getSocratesX()
{
    return m_socrates->getX();
}

int StudentWorld::getSocratesY()
{
    return m_socrates->getY();
}

void StudentWorld::damageSocrates(int amt)
{
    m_socrates->takeDamage(amt);
    if (m_socrates->getHealth() <= 0)
        playSound(SOUND_PLAYER_DIE);
    else
        playSound(SOUND_PLAYER_HURT);
    return;
}

bool StudentWorld::findClosestFood(int x, int y, Direction &dir, int &newX, int &newY)
{
    bool foundFood = false;
    list<Actor*>::iterator it = m_actors.begin();
    for (list<Actor*>::iterator i = m_actors.begin(); i != m_actors.end(); i++) {
        if (!(*i)->isEdible()) continue;
        double distance1 = sqrt(pow(((*i)->getX())-x, 2) + pow((*i)->getY()-y, 2));
        double distance2 = sqrt(pow((*it)->getX()-x, 2) + pow((*it)->getY()-y, 2));
        if (distance1 > 128) continue;
        if (distance1 <= distance2)
        {
            it = i;
            foundFood = true;
        }
    }
    if (foundFood)
    {
        double angle = (180/PI) * atan2(((*it)->getY()-y), ((*it)->getX()-x));
        // if (((*it)->getY()-y) < 0 && ((*it)->getX()-x) < 0) angle += 180;
        // below is wrong!!!
        newX = x + 3 * cos(angle * PI / 180);
        newY = y + 3 * sin(angle * PI / 180);
        dir = angle;
    }
    return foundFood;
}

bool StudentWorld::aggNearSocrates(int x, int y, Direction &dir, int distance)
{
    bool near = false;
    if (sqrt(pow(x - m_socrates->getX(), 2) + pow(y - m_socrates->getY(), 2)) <= distance)
    {
        near = true;
        // FIX THIS!
        double angle = 180/PI * atan2((m_socrates->getY()-y), (m_socrates->getX()-x));
        //if ((m_socrates->getY()-y) < 0 && (m_socrates->getX()-x) < 0) angle += 180;
        dir = angle;
    }
    return near;
}

Socrates* StudentWorld::getOverlappingSocrates(Actor* a) const
{
    double distance = sqrt(pow((m_socrates->getX() * 1.0) - (a->getX() * 1.0), 2) + pow((m_socrates->getY() * 1.0) - (a->getY() * 1.0), 2));
    if (distance <= SPRITE_WIDTH)
    {
        return m_socrates;
    }
    return nullptr;
}

  // If actor a overlaps a living edible object, return a pointer to the
  // edible object; otherwise, return nullptr
Actor* StudentWorld::getOverlappingEdible(Actor* a) const
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->isEdible()) {
            double distance = sqrt(pow(((*it)->getX() * 1.0) - (a->getX() * 1.0), 2) + pow(((*it)->getY() * 1.0) - (a->getY() * 1.0), 2));
            if (distance <= SPRITE_WIDTH)
            {
                return (*it);
            }
        }
    }
    return nullptr;
}

bool StudentWorld::isBacteriumMovementBlockedAt(int x, int y) const
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->blocksBacteriumMovement()) {
            double distance = sqrt(pow(((*it)->getX() * 1.0) - (x * 1.0), 2) + pow(((*it)->getY() * 1.0) - (y * 1.0), 2));
            if (distance <= SPRITE_WIDTH/2)
            {
                return true;
            }
        }
    }
    return false;
}

  // If actor a ovelaps some live actor, damage that live actor by the
  // indicated amount of damage and return true; otherwise, return false.
bool StudentWorld::damageOneActor(Actor* a, int damage)
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        double distance = sqrt(pow(((*it)->getX() * 1.0) - (a->getX() * 1.0), 2) + pow(((*it)->getY() * 1.0) - (a->getY() * 1.0), 2));
        if (distance <= SPRITE_WIDTH)
        {
            if ((*it)->takeDamage(damage)) return true;
        }
    }
    return false;
}


  // Add an actor to the world.
void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
    return;
}
