#ifndef MOB_H
#define MOB_H

#include "irrlicht.h"
#include "state.h"
#include <memory>

class State;

class Mob
{
	State * mState;

public:
	Mob();

	virtual ~Mob(){};
    virtual void Init();
	virtual void Update();

	void SetState(State * newState);
	void OnMessage(std::string * message);
	void ApplyVectors();

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