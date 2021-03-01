#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//TODO

// DO FUNGUS

// FIX DAMAGE STUFF

// GOODIES!

// FIX STRINGSTREAM ON STAT BAR (SPACES)

// CHECK SPEC FOR OTHER STUFF

// AUXILARY FUNCTIONS

bool willHitBorder(int x, int y)
{
    // Compare radius of circle with distance
    // of its center from given point
    if ((x - VIEW_HEIGHT/2) * (x - VIEW_HEIGHT/2) +
        (y - VIEW_WIDTH/2) * (y - VIEW_WIDTH/2) <= VIEW_RADIUS * VIEW_RADIUS)
        return false;
    return true;
}

// ACTOR IMPLEMENTATION

Actor::Actor(StudentWorld* sWorld, int imageID, int startX, int startY, Direction startDir, int depth) : GraphObject(imageID, startX, startY, startDir, depth), m_world(sWorld), m_living(true)
{
}

StudentWorld* Actor::getWorld() const
{
    return m_world;
}

void Actor::setAsDead() {
    m_living = false;
}

bool Actor::alive() const
{
    return m_living;
}

bool Actor::takeDamage(int damage)
{
    return false;
}

bool Actor::blocksBacteriumMovement() const
{
    return false;
}

bool Actor::isEdible() const
{
    return false;
}

bool Actor::preventsLevelCompleting() const
{
    return false;
}

// AGENT IMPLEMENTATION

Agent::Agent(StudentWorld* sWorld, int hitpoints, int imageID, int startX, int startY, Direction startDir, int depth) : Actor(sWorld, imageID, startX, startY, startDir, depth), m_hitpoints(hitpoints)
{
}

int Agent::getHealth()
{
    return m_hitpoints;
}

void Agent::restoreHealth(int amount)
{
    m_hitpoints = amount;
    return;
}

bool Agent::takeDamage(int damage)
{
    m_hitpoints -= damage;
    if (m_hitpoints <= 0)
        setAsDead();
    return true;
}

// SOCRATES IMPLEMENTATION

Socrates::Socrates(StudentWorld* sWorld, int startX, int startY, Direction startDir, int depth) : Agent(sWorld, 100, IID_PLAYER, startX, startY, startDir, depth), m_spray(20), m_flame(5)
{
}

void Socrates::doSomething()
{
    if (getHealth() <= 0) return;
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        double pi = atan(1) * 4;
        switch (ch)
        {
            case KEY_PRESS_LEFT: { // MOVE COUNTER CLOCKWISE
                double x = 128 + 128 *  cos((getDirection()+5-180) * 1.0 / 360 * 2 * pi);
                double y = 128 + 128 *  sin((getDirection()+5-180) * 1.0 / 360 * 2 * pi);
                moveTo(x, y);
                setDirection(getDirection()+5);
                break;
            }
            case KEY_PRESS_RIGHT: { // MOVE CLOCKWISE
                double x = 128 + 128 *  cos((getDirection()-5-180) * 1.0 / 360 * pi * 2);
                double y = 128 + 128 *  sin((getDirection()-5-180) * 1.0 / 360 * pi * 2);
                moveTo(x, y);
                setDirection(getDirection()-5);
                break;
            }
            case KEY_PRESS_SPACE: {
                if (m_spray <= 0) break;
                // SPRAY OBJECT!!
                int newX = getX() + SPRITE_WIDTH * cos(getDirection() * PI / 180);
                int newY = getY() + SPRITE_WIDTH * sin(getDirection() * PI / 180);
                getWorld()->addActor(new Spray(getWorld(), newX, newY, getDirection()));
                m_spray--;
                getWorld()->playSound(SOUND_PLAYER_SPRAY);
                break;
            }
            case KEY_PRESS_ENTER: {
                if (m_flame <= 0) break;
                // FLAME OBJECT!!
                for (int i = 0; i < 16; i++)
                {
                    int newDir = getDirection()+(i*22);
                    int newX = getX() + SPRITE_WIDTH * cos(newDir * PI / 180);
                    int newY = getY() + SPRITE_WIDTH * sin(newDir * PI / 180);
                    getWorld()->addActor(new Flame(getWorld(), newX, newY, newDir));
                }
                m_flame--;
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                break;
            }
        }
    } else {
        if (m_spray < 20) m_spray++;
    }
}

int Socrates::getSprays()
{
    return m_spray;
}

int Socrates::getFlames()
{
    return m_flame;
}

void Socrates::addFlames(int amount)
{
    m_flame += amount;
    return;
}

int Socrates::soundWhenHurt() const
{
    return SOUND_PLAYER_HURT;
}
int Socrates::soundWhenDie() const
{
    return SOUND_PLAYER_DIE;
}

// BACTERIUM IMPLEMENTATION

Bacterium::Bacterium(StudentWorld* sWorld, int hitpoints, int movePlanDistance, int imageID, int startX, int startY, Direction startDir, int depth) : Agent(sWorld, hitpoints, imageID, startX, startY, startDir, depth), m_movePlanDistance(movePlanDistance), m_foodCount(0)
{
}

void Bacterium::doSomething()
{
    if (!alive()) return;
    if(specificDoSomething())
    {
        if (getWorld()->getOverlappingSocrates(this) != nullptr) return;
        if (getMovePlanDistance() > 0)
        {
            setMovePlanDistance(getMovePlanDistance()-1); // decrement plan distance by 1
            int newX = getX() + 3 * cos(getDirection() * PI / 180);
            int newY = getY() + 3 * sin(getDirection() * PI / 180);
            // std::cout << "(" << newX << ", " << newY << ")" << std::endl;
            if (getWorld()->isBacteriumMovementBlockedAt(newX, newY) || willHitBorder(newX, newY))
            {
                setRandomDirection();
                setMovePlanDistance(10);
                return;
            };
            // MOVE HERE!!!!
            moveTo(newX, newY);
            return;
        }
        else {
            int targetX, targetY;
            Direction targetDir;
            if(getWorld()->findClosestFood(getX(), getY(), targetDir, targetX, targetY) && !getWorld()->isBacteriumMovementBlockedAt(targetX, targetY) && !willHitBorder(targetX, targetY))
            {
                // validify direction
                setDirection(targetDir);
                setMovePlanDistance(10);
                return;
            }
            setRandomDirection();
            setMovePlanDistance(10);
            targetX = getX() + 3 * cos(getDirection() * PI / 180);
            targetY = getY() + 3 * sin(getDirection() * PI / 180);
            return;
        }
    }
}

int Bacterium::getFoodCount()
{
    return m_foodCount;
}

void Bacterium::incFoodCount()
{
    m_foodCount++;
    return;
}

void Bacterium::resetFoodCount()
{
    m_foodCount = 0;
    return;
}

int Bacterium::getMovePlanDistance()
{
    return m_movePlanDistance;
}

void Bacterium::setMovePlanDistance(int distance)
{
    m_movePlanDistance = distance;
    return;
}

void Bacterium::setRandomDirection()
{
    int newDir = randInt(0, 359);
    setDirection(newDir);
    return;
}

bool Bacterium::takeDamage(int damage)
{
    if (!alive()) return true;
    Agent::takeDamage(damage);
    if (getHealth() <= 0)
    {
        getWorld()->playSound(soundWhenDie());
        getWorld()->increaseScore(100);
        if (randInt(1, 2) == 2)
            getWorld()->addActor(new Food(getWorld(), getX(), getY()));
        setAsDead();
    }
    else
        getWorld()->playSound(soundWhenHurt());
    return true;
}

bool Bacterium::preventsLevelCompleting() const
{
    return true;
}

// SALMONELLA IMPLEMENTATION

Salmonella::Salmonella(StudentWorld* sWorld, int startX, int startY) :  Bacterium(sWorld, 4, 0, IID_SALMONELLA, startX, startY, 90, 0)
{
}

bool Salmonella::specificDoSomething()
{
    if (getWorld()->getOverlappingSocrates(this) != nullptr) {
        getWorld()->damageSocrates(1);
    }
    else if (getFoodCount() == 3)
    {
        int newX = getX() < VIEW_WIDTH/2 ? getX()+SPRITE_WIDTH/2 : (getX() > VIEW_WIDTH/2 ? getX()-SPRITE_WIDTH/2 : getX());
        int newY = getY() < VIEW_HEIGHT/2 ? getY()+SPRITE_WIDTH/2 : (getY() > VIEW_HEIGHT/2 ? getX()-SPRITE_WIDTH/2 : getY());
        getWorld()->createSalmonella(newX, newY);
        resetFoodCount();
    }
    else if (getWorld()->getOverlappingEdible(this) != nullptr)
    {
        incFoodCount();
        getWorld()->getOverlappingEdible(this)->setAsDead();
    }
    return true;
}

int Salmonella::soundWhenHurt() const
{
    return SOUND_SALMONELLA_HURT;
}
int Salmonella::soundWhenDie() const
{
    return SOUND_SALMONELLA_DIE;
}

// AGGRESSIVE SALMONELLA IMPLEMENTATION

AggressiveSalmonella::AggressiveSalmonella(StudentWorld* sWorld, int startX, int startY) : Bacterium(sWorld, 10, 0, IID_SALMONELLA, startX, startY, 90, 0)
{
}

bool AggressiveSalmonella::specificDoSomething()
{
    // stuff here don't forget
    bool stepSix = true; // should do steps 3-5!
    Direction dir;
    if (getWorld()->aggNearSocrates(getX(), getY(), dir, 72))
    {
        int newX = getX() + 3 * cos(dir * PI / 180);
        int newY = getY() + 3 * sin(dir * PI / 180);
//        if (willHitBorder(newX, newY) && !getWorld()->isBacteriumMovementBlockedAt(newX, newY))
//        {
//           stepSix = true;
//        }
        if (!getWorld()->isBacteriumMovementBlockedAt(newX, newY))
        {
            setDirection(dir);
            moveTo(newX, newY);
        }
        stepSix = false; // nah, skip step six.
    }
    if (getWorld()->getOverlappingSocrates(this) != nullptr)
    {
        getWorld()->damageSocrates(2);
    }
    else if (getFoodCount() == 3)
    {
        int newX, newY;
        if (getX() < VIEW_WIDTH/2)
            newX = getX()+SPRITE_RADIUS;
        else if (getX() > VIEW_WIDTH/2)
            newX = getX()-SPRITE_RADIUS;
        else
            newX = getX();
        
        if (getY() < VIEW_WIDTH/2)
            newY = getY()+SPRITE_RADIUS;
        else if (getY() > VIEW_WIDTH/2)
            newY = getY()-SPRITE_RADIUS;
        else
            newY = getY();
        if (pow(newX, 2) + pow(newY, 2) > pow(VIEW_RADIUS, 2))
        {
            newX = getX();
            newY = getY();
        }
        getWorld()->createSalmonella(newX, newY);
        resetFoodCount();
    }
    else if (getWorld()->getOverlappingEdible(this) != nullptr)
    {
        incFoodCount();
        getWorld()->getOverlappingEdible(this)->setAsDead();
    }
    return stepSix;
}

int AggressiveSalmonella::soundWhenHurt() const
{
    return SOUND_SALMONELLA_HURT;
}
int AggressiveSalmonella::soundWhenDie() const
{
    return SOUND_SALMONELLA_DIE;
}

// E COLI IMPLEMENTATION

EColi::EColi(StudentWorld* sWorld, int startX, int startY) : Bacterium(sWorld, 5, 0, IID_ECOLI, startX, startY, 90, 0)
{
}

bool EColi::specificDoSomething()
{
    Direction dir;
    if (getWorld()->getOverlappingSocrates(this) != nullptr) {
        getWorld()->damageSocrates(4);
    }
    else if (getFoodCount() == 3)
    {
        int newX = getX() < VIEW_WIDTH/2 ? getX()+SPRITE_WIDTH/2 : (getX() > VIEW_WIDTH/2 ? getX()-SPRITE_WIDTH/2 : getX());
        int newY = getY() < VIEW_HEIGHT/2 ? getY()+SPRITE_WIDTH/2 : (getY() > VIEW_HEIGHT/2 ? getX()-SPRITE_WIDTH/2 : getY());
        getWorld()->createEColi(newX, newY);
        resetFoodCount();
    }
    else if (getWorld()->getOverlappingEdible(this) != nullptr)
    {
        incFoodCount();
        getWorld()->getOverlappingEdible(this)->setAsDead();
    }
    else if (getWorld()->aggNearSocrates(getX(), getY(), dir, 256))
    {
        for (int i = 0; i < 10; i++)
        {
            int newX = getX() + 2 * cos((dir + (10 * i)) * PI / 180);
            int newY = getY() + 2 * sin((dir + (10 * i)) * PI / 180);
            if (!getWorld()->isBacteriumMovementBlockedAt(newX, newY) && !willHitBorder(newX, newY))
            {
                //std::cout << i << std::endl;
                setDirection(dir + (10 * i));
                moveTo(newX, newY);
                return false;
            }
        }
    }
    return false;
}

int EColi::soundWhenHurt() const
{
    return SOUND_ECOLI_HURT;
}
int EColi::soundWhenDie() const
{
    return SOUND_ECOLI_DIE;
}

// PIT IMPLEMENTATION

Pit::Pit(StudentWorld* sWorld, int startX, int startY) : Actor(sWorld, IID_PIT, startX, startY, 0, 1), m_salmonella(5), m_aggSalmonella(3), m_eColi(2)
{
}

void Pit::doSomething()
{
    if (m_salmonella == 0 & m_aggSalmonella == 0 && m_eColi == 0) {
        // inform StudentWorld!
        setAsDead();
        return;
    }
    int i = randInt(1, 50);
    if (i == 1) {
        std::string remaining;
        if (m_salmonella != 0) remaining += 's';
        if (m_aggSalmonella != 0) remaining += 'a';
        if (m_eColi != 0) remaining += 'e';
        int fin;
        for (fin=0; fin<remaining.size(); fin++) {};
        int j = randInt(0, fin-1);
        char search = remaining[j];
        switch(search) {
            case 's': {
                getWorld()->createSalmonella(getX(), getY());
                m_salmonella--;
                break;
            }
            case 'a': {
                getWorld()->createAggSalmonella(getX(), getY());
                m_aggSalmonella--;
                break;
            }
            case 'e': {
                
                getWorld()->createEColi(getX(), getY());
                m_eColi--;
                break;
            }
        }
        getWorld()->playSound(SOUND_BACTERIUM_BORN);
    }
    return;
}

bool Pit::preventsLevelCompleting() const
{
    return true;
}

// DIRT IMPLEMENTATION

Dirt::Dirt(StudentWorld* sWorld, int startX, int startY) : Actor(sWorld, IID_DIRT, startX, startY, 0, 1)
{
}

void Dirt::doSomething()
{
    // Ha! It's dirt! What else can it do?!
    return;
}

bool Dirt::blocksBacteriumMovement() const
{
    return true;
}

bool Dirt::takeDamage(int damage)
{
    setAsDead();
    return true;
}

// ACTIVATING OBJECT IMPLEMENTATION

ActivatingObject::ActivatingObject(StudentWorld* sWorld, int imageID, int startX, int startY,  Direction startDir, int depth) : Actor(sWorld, imageID, startX, startY, startDir, depth)
{
}

// FOOD IMPLEMENTATION

Food::Food(StudentWorld* sWorld, int startX, int startY) : ActivatingObject(sWorld, IID_FOOD, startX, startY, 90, 1)
{
}

void Food::doSomething()
{
    // I guess I won't decay.
    return;
}

bool Food::isEdible() const
{
    return true;
}


// PROJECTILE IMPLEMENTATION

Projectile::Projectile(StudentWorld* sWorld, int flightDistance, int imageID, int startX, int startY, Direction startDir, int depth) : ActivatingObject(sWorld, imageID, startX, startY, startDir, depth), m_flightDistance(flightDistance)
{
}

int Projectile::getFlightDistance()
{
    return m_flightDistance;
}

void Projectile::decFlightDistance()
{
    m_flightDistance -= SPRITE_WIDTH;
    return;
}

// FLAME IMPLEMENTATION

Flame::Flame(StudentWorld* sWorld, int startX, int startY, Direction startDir) : Projectile(sWorld, 32, IID_FLAME, startX, startY, startDir, 1)
{
}

void Flame::doSomething() {
    if (!alive()) return;
    if (getFlightDistance() == 0) {
        setAsDead();
        return;
    }
    if (getWorld()->damageOneActor(this, 5)) setAsDead();
    
    int newX = getX() + SPRITE_WIDTH * cos(getDirection() * PI / 180);
    int newY = getY() + SPRITE_WIDTH * sin(getDirection() * PI / 180);
    moveTo(newX, newY);
    decFlightDistance();
}

// SPRAY IMPLEMENTATION

Spray::Spray(StudentWorld* sWorld, int startX, int startY, Direction startDir) : Projectile(sWorld, 112, IID_SPRAY, startX, startY, startDir, 1)
{
}

void Spray::doSomething()
{
    if (!alive()) return;
    if (getFlightDistance() == 0) {
        setAsDead();
        return;
    }
    if (getWorld()->damageOneActor(this, 2)) setAsDead();
    
    int newX = getX() + SPRITE_WIDTH * cos(getDirection() * PI / 180);
    int newY = getY() + SPRITE_WIDTH * sin(getDirection() * PI / 180);
    moveTo(newX, newY);
    decFlightDistance();
}

// GOODIE IMPLEMENTATION

Goodie::Goodie(StudentWorld* sWorld, int imageID, double startX, double startY) : Actor(sWorld, imageID, startX, startY, 0, 1)
{
    m_lifetime = max(randInt(0, 300 - 10 * getWorld()->getLevel() - 1), 50);
}

bool Goodie::takeDamage(int damage)
{
    setAsDead();
    return true;
}

void Goodie::decLifetime()
{
    m_lifetime--;
    return;
}

int Goodie::getLifetime()
{
    return m_lifetime;
}

// RESTORE HEALTH IMPLEMENTATION

RestoreHealthGoodie::RestoreHealthGoodie(StudentWorld* sWorld, double startX, double startY) : Goodie(sWorld, IID_RESTORE_HEALTH_GOODIE, startX, startY)
{
}
void RestoreHealthGoodie::doSomething()
{
    if (!alive()) return;
    decLifetime();
    if (getWorld()->getOverlappingSocrates(this) != nullptr)
    {
        getWorld()->increaseScore(250);
        takeDamage(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getOverlappingSocrates(this)->restoreHealth(100);
        return;
    }
    if (getLifetime() <= 0) setAsDead();
}

// FLAME THROWER GOODIE IMPLEMENTATION

FlamethrowerGoodie::FlamethrowerGoodie(StudentWorld* sWorld, double startX, double startY) : Goodie(sWorld, IID_FLAME_THROWER_GOODIE, startX, startY)
{
}

void FlamethrowerGoodie::doSomething()
{
    if (!alive()) return;
    decLifetime();
    if (getWorld()->getOverlappingSocrates(this) != nullptr)
    {
        getWorld()->increaseScore(300);
        takeDamage(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getOverlappingSocrates(this)->addFlames(5);
        return;
    }
    if (getLifetime() <= 0) setAsDead();
}

// EXTRA LIFE GOODIE IMPLEMENTATION

ExtraLifeGoodie::ExtraLifeGoodie(StudentWorld* sWorld, double startX, double startY) : Goodie(sWorld, IID_EXTRA_LIFE_GOODIE, startX, startY)
{
}

void ExtraLifeGoodie::doSomething()
{
    if (!alive()) return;
    decLifetime();
    if (getWorld()->getOverlappingSocrates(this) != nullptr)
    {
        getWorld()->increaseScore(500);
        takeDamage(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->incLives();
        return;
    }
    if (getLifetime() <= 0) setAsDead();
}

// FUNGUS IMPLEMENTATION

Fungus::Fungus(StudentWorld* sWorld, double startX, double startY) : Goodie(sWorld, IID_FUNGUS, startX, startY)
{
}

void Fungus::doSomething()
{
    if (!alive()) return;
    decLifetime();
    if (getWorld()->getOverlappingSocrates(this) != nullptr)
    {
        getWorld()->increaseScore(-50);
        takeDamage(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getOverlappingSocrates(this)->takeDamage(20);
        return;
    }
    if (getLifetime() <= 0) setAsDead();
}
