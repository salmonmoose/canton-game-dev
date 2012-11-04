#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "engine.h"
#include "mob.h"
#include "pewpew.h"
#include "mouse.h"

class Player : public Mob
{
public:

    irr::scene::IParticleSystemSceneNode * playerEngine1;
    irr::scene::IParticleSystemSceneNode * playerEngine2;
    
    irr::scene::IParticleEmitter * playerEngineEmitter1;
    irr::scene::IParticleEmitter * playerEngineEmitter2;

    Mouse mouse;

    Player();
    ~Player();
    void Update();
    float getAngleToMouse();

protected:
};

#endif