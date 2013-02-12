#ifndef MOB_H
#define MOB_H

#include "irrlicht.h"
#include "State.h"
#include "Ability.h"
#include "Factory.h"
#include "Stats.h"
#include <memory>
#include <vector>

class State;
class Ability;

class Mob
{
public:
	Mob(bool Visible, bool Solid, bool Mobile);

	virtual ~Mob(){};
    virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void Collide(const std::vector<std::shared_ptr<Mob>> *);
	virtual void Collide(const std::shared_ptr<Mob>);

	void SetState(State * newState);
	void OnMessage(std::string * message);
	void ApplyVectors();

	void SetPosition( const irr::core::vector3df _position ){ Position = _position; }
	void SetRotation( const irr::core::vector3df _rotation ){ Rotation = _rotation; }
	void SetVelocity( const irr::core::vector3df _velocity ){ Velocity = _velocity; }

	void AddAbility(std::string);
	void TriggerAbility(std::string);

	const irr::core::vector3df & getPosition(){ return Position; }
	const irr::core::vector3df & getRotation(){ return Rotation; }
	const irr::core::vector3df & getVelocity(){ return Velocity; }

	const irr::core::aabbox3df & getBoundingBox(){ return mainMesh->getBoundingBox(); }
	const irr::core::aabbox3df & getTransformedBoundingBox(){ return mainMesh->getTransformedBoundingBox(); }

	virtual const irr::core::vector3df & getTargetPosition();

	const float getAngleToVector(irr::core::vector3df);
	const float getDistanceToVector(irr::core::vector3df);

	irr::scene::IAnimatedMeshSceneNode * mainMesh;

	Stats mStats;

	std::map<std::string, std::shared_ptr<Ability>> * mapAbility;

    std::vector<std::shared_ptr<Ability>> * vAbility;
    std::vector<std::shared_ptr<Ability>>::iterator vAbilityIterator;

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

	double Lift;
	double Gravity;
};

#endif