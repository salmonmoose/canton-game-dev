#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "mob.h"
#include "mouse.h"

class Mouse;

class Player : public Mob
{
public:

    irr::scene::IParticleSystemSceneNode * playerEngine1;
    irr::scene::IParticleSystemSceneNode * playerEngine2;
    
    irr::scene::IParticleEmitter * playerEngineEmitter1;
    irr::scene::IParticleEmitter * playerEngineEmitter2;

    Mouse * mouse;

    float TurnBuffer;

    Player();
    ~Player();
    void Init();
    void Update();
    void AcceptInput();
    float getAngleToMouse();

protected:
};

#endif