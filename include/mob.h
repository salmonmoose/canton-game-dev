#ifndef MOB_H
#define MOB_H

#include "irrlicht.h"
#include "engine.h"

class Mob
{
public:
	Mob()
	{
		//mainMesh = IRR.smgr->addCubeSceneNode();
	}


	~Mob(){};

	void Update()
	{
		Velocity -= Velocity * Drag * IRR.frameDeltaTime;

		if(Velocity.getLength() > MaxSpeed) Velocity.setLength(MaxSpeed);

		Position += IRR.frameDeltaTime * Velocity;

		mainMesh->setPosition(Position);
		mainMesh->setRotation(Rotation);
	}

	const irr::core::vector3df & getPosition(){ return Position; }
	const irr::core::vector3df & getRotation(){ return Rotation; }
	const irr::core::vector3df & getVelocity(){ return Velocity; }

	irr::scene::IAnimatedMeshSceneNode * mainMesh;

protected:
	irr::core::vector3df Position;
	irr::core::vector3df Velocity;
	irr::core::vector3df Rotation;
	irr::core::matrix4 Matrix;

	double MaxSpeed;
	double MaxStrafe;
	double MaxTurnRate;
	double Drag;
};

#endif