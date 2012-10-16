#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "engine.h"
#include "mob.h"

class Player : public Mob
{
public:

    irr::scene::IParticleSystemSceneNode * playerEngine;
    irr::scene::IParticleEmitter * playerEngineEmitter;
    Player();
    ~Player();
    void Update();

protected:
};

#endif