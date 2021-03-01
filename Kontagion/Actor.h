#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//GraphObject
//Actor
//    Dirt
//    Pit
//    ActivatingObject
//        Food
//        Flame
//        Spray
//        Goodie
//            RestoreHealthGoodie
//            FlamethrowerGoodie
//            ExtraLifeGoodie
//            Fungus
//    Agent
//        Socrates
//        Bacterium
//            Salmonella
//                RegularSalmonella
//                AggressiveSalmonella

const int REGULAR_SALMONELLA_DAMAGE = 1;
const int AGGRESSIVE_SALMONELLA_DAMAGE = 2;

const double PI = 3.14159265358979323846264338327;

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sWorld, int imageID, int startX, int startY, Direction startDir, int depth);
    virtual void doSomething() = 0;
    void setAsDead();
    bool alive() const;
    
    virtual bool takeDamage(int damage);
    
    virtual bool blocksBacteriumMovement() const; // false except for dirt
      // Is this object edible?
    virtual bool isEdible() const;
    virtual bool preventsLevelCompleting() const;
protected:
    StudentWorld* getWorld() const;
private:
    StudentWorld* m_world;
    bool m_living;
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sWorld, int hitpoints, int imageID, int startX, int startY, Direction startDir, int depth);
    virtual void doSomething() = 0;
    int getHealth();
    void restoreHealth(int health);
    
    virtual bool takeDamage(int damage);
    
    virtual int soundWhenHurt() const = 0;
    virtual int soundWhenDie() const = 0;
private:
    int m_hitpoints;
};

class Socrates : public Agent
{
public:
    Socrates(StudentWorld* sWorld, int startX, int startY, Direction startDir, int depth);
    virtual void doSomething();
    
    int getFlames();
    int getSprays();
    void addFlames(int amount);
    
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
private:
    int m_spray;
    int m_flame;
};

class Bacterium : public Agent
{
public:
    Bacterium(StudentWorld* sWorld, int hitpoints, int movePlanDistance, int imageID, int startX, int startY, Direction startDir, int depth);
    virtual void doSomething();
    virtual bool specificDoSomething() = 0;
    
    int getFoodCount();
    void incFoodCount();
    void resetFoodCount();
    
    int getMovePlanDistance();
    void setMovePlanDistance(int distance);
    
    void setRandomDirection();
    void move(int x, int y);
    
    virtual bool takeDamage(int damage);
    
    virtual bool preventsLevelCompleting() const;
private:
    int m_movePlanDistance;
    int m_foodCount;
};

class Salmonella : public Bacterium
{
public:
    Salmonella(StudentWorld* sWorld, int startX, int startY);
    virtual bool specificDoSomething();
    
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
};

class AggressiveSalmonella : public Bacterium
{
public:
    AggressiveSalmonella(StudentWorld* sWorld, int startX, int startY);
    virtual bool specificDoSomething();
    
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
};

class EColi : public Bacterium
{
public:
    EColi(StudentWorld* sWorld, int startX, int startY);
    virtual bool specificDoSomething();
    
    virtual int soundWhenHurt() const;
    virtual int soundWhenDie() const;
};

class Pit : public Actor
{
public:
    Pit(StudentWorld* sWorld, int startX, int startY);
    virtual void doSomething();
    virtual bool preventsLevelCompleting() const;
private:
    int m_salmonella;
    int m_aggSalmonella;
    int m_eColi;
};

class Dirt : public Actor
{
public:
    Dirt(StudentWorld* sWorld, int startX, int startY);
    virtual void doSomething();
    virtual bool takeDamage(int damage);
    
    virtual bool blocksBacteriumMovement() const; // yes!
};

class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* sWorld, int imageID, int startX, int startY, Direction startDir, int depth);
    virtual void doSomething() = 0;
};

class Food : public ActivatingObject
{
public:
    Food(StudentWorld* sWorld, int startX, int startY);
    virtual void doSomething();
      // Is this object edible?
    virtual bool isEdible() const;
};

class Projectile : public ActivatingObject
{
public:
    Projectile(StudentWorld* sWorld, int flightDistance, int imageID, int startX, int startY, Direction startDir, int depth);
    virtual void doSomething() = 0;
    
    int getFlightDistance();
    void decFlightDistance();
private:
    int m_flightDistance;
};

class Flame : public Projectile
{
public:
    Flame(StudentWorld* sWorld, int startX, int startY, Direction startDir);
    virtual void doSomething();
};

class Spray : public Projectile
{
public:
    Spray(StudentWorld* sWorld, int startX, int startY, Direction startDir);
    virtual void doSomething();
};

class Goodie : public Actor
{
public:
    Goodie(StudentWorld* sWorld, int imageID, double x, double y);
    virtual bool takeDamage(int damage);

      // Have s pick up this goodie.
    void decLifetime();
    int getLifetime();
private:
    int m_lifetime;
};

class RestoreHealthGoodie : public Goodie
{
public:
    RestoreHealthGoodie(StudentWorld* sWorld, double x, double y);
    virtual void doSomething();
};

class FlamethrowerGoodie : public Goodie
{
public:
    FlamethrowerGoodie(StudentWorld* sWorld, double x, double y);
    virtual void doSomething();
};

class ExtraLifeGoodie : public Goodie
{
public:
    ExtraLifeGoodie(StudentWorld* sWorld, double x, double y);
    virtual void doSomething();
};

class Fungus : public Goodie
{
public:
    Fungus(StudentWorld* sWorld, double x, double y);
    virtual void doSomething();
};

#endif // ACTOR_H_
