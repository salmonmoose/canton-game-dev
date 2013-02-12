#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "Mob.h"
#include "Mouse.h"
#include "Stats.h"

class Mouse;

class Player : public Mob
{
public:

    Mouse * mouse;

    float TurnBuffer;
    double Lift;
    double Gravity;

    Player();
    ~Player();
    void Init();
    void AcceptInput();
    float getAngleToMouse();

    const irr::core::vector3df & getTargetPosition();

protected:
};

#endif