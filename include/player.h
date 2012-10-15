#ifndef PLAYER_H
#define PLAYER_H

#include "irrlicht.h"
#include "engine.h"

class Player
{
public:

    Player();

    ~Player();

    void Update();

    const irr::core::vector3df & getPosition();
    const irr::core::vector3df & getRotation();
    const irr::core::vector3df & getVelocity();
	irr::scene::IAnimatedMeshSceneNode * playerMesh;
    irr::scene::IParticleSystemSceneNode * playerEngine;
    irr::scene::IParticleEmitter * playerEngineEmitter;

protected:
	irr::core::vector3df Position;
	irr::core::vector3df Velocity;
    irr::core::vector3df Rotation;
    irr::core::matrix4 Matrix;

    double Speed;
    double Strafe;
    double Steering;
    double Drag;
};

#endif