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
	irr::scene::IAnimatedMeshSceneNode * playerMesh;
    irr::core::vector3df getIrrIn();

protected:
	irr::core::vector3df Position;
	irr::core::vector3df Velocity;
	irr::core::vector3df Rotation;
    irr::core::matrix4 Matrix;

    double Speed;
};

#endif