#ifndef ABILITY_H
#define ABILITY_H

#include "irrlicht.h"
#include "Engine.h"
#include "Mob.h"

class Mob;

class Ability
{
public:
    Ability();

    virtual ~Ability(){};
    virtual void Initialize(Mob *);
    virtual void Update();
    virtual void Trigger();

    const int GetCoolDown();
    bool Ready();

private:
    int Rate;
    int LastTick;

    Mob * mMob;
};

#endif