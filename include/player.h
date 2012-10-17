#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "engine.h"
#include "mob.h"

class Player : public Mob
{
public:

    irr::scene::IParticleSystemSceneNode * playerEngine1;
    irr::scene::IParticleSystemSceneNode * playerEngine2;
    
    irr::scene::IParticleEmitter * playerEngineEmitter1;
    irr::scene::IParticleEmitter * playerEngineEmitter2;
    Player();
    ~Player();
    void Update();

protected:
};

#endif