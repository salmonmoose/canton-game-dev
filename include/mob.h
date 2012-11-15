#ifndef MOB_H
#define MOB_H

#include "irrlicht.h"
#include "state.h"
#include "factory.h"
#include <memory>

class State;

class Mob
{
public:
	Mob(bool Visible, bool Solid, bool Mobile);

	virtual ~Mob(){};
    virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void Collide(Mob mobs[]);

	void SetState(State * newState);
	void OnMessage(std::string * message);
	void ApplyVectors();

	const irr::core::vector3df & getPosition(){ return Position; }
	const irr::core::vector3df & getRotation(){ return Rotation; }
	const irr::core::vector3df & getVelocity(){ return Velocity; }

	irr::scene::IAnimatedMeshSceneNode * mainMesh;

protected:
	State * mState;

	bool _Visible;
	bool _Solid;
	bool _Mobile;

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