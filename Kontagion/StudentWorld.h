#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GraphObject.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

using ObjectType = int;

class Actor;
class Socrates;
class Dirt;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual ~StudentWorld();
    
    void setStatus();
    
      // Add an actor to the world.
    void addActor(Actor* a);
    
      // If actor a ovelaps some live actor, damage that live actor by the
      // indicated amount of damage and return true; otherwise, return false.
    bool damageOneActor(Actor* a, int damage);

    void createSalmonella(int x, int y);
    void createAggSalmonella(int x, int y);
    void createEColi(int x, int y);
    
    int getSocratesX();
    int getSocratesY();
    
    void damageSocrates(int amt);
      // Is bacterium a blocked from moving to the indicated location?
    bool isBacteriumMovementBlockedAt(int x, int y) const;
      // If actor a overlaps this world's socrates, return a pointer to the
      // socrates; otherwise, return nullptr
    Socrates* getOverlappingSocrates(Actor* a) const;

      // If actor a overlaps a living edible object, return a pointer to the
      // edible object; otherwise, return nullptr
    Actor* getOverlappingEdible(Actor* a) const;
    void eatFood(int x, int y);
    
    bool findClosestFood(int x, int y, Direction &dir, int &newX, int &newY);
    bool aggNearSocrates(int x, int y, Direction &dir, int distance);
private:
    std::list<Actor*> m_actors;
    Socrates* m_socrates;
};

#endif // STUDENTWORLD_H_
