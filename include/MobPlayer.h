#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "Mob.h"
#include "Mouse.h"

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
    double Lift;
    double Gravity;

    Player();
    ~Player();
    void Init();
    void AcceptInput();
    float getAngleToMouse();

protected:
};

#endif