#ifndef PEWPEW_H
#define PEWPEW_H

#include "irrlicht.h"
#include "engine.h"
#include "mob.h"
#include "shadercallback.h"
#include "state.h"

class PewPew : public Mob
{
public:
	irr::scene::IParticleSystemSceneNode * particles;
	irr::scene::IParticleEmitter * emitter;
    irr::io::path vsFileName;
    irr::io::path psFileName;
    irr::video::SMaterial Material;
    irr::s32 MatID;
	PewPew(irr::core::vector3df pos, irr::core::vector3df rot, irr::core::vector3df vel);
	~PewPew(){};
	void Update();
    void Init();
	double life;
};

class ShipPatrolState : public State
{
	void OnUpdate()
	{

	}

	void OnEnter()
	{

	}

	void OnLeave()
	{

	}

	void OnMessage()
	{
		
	}
};

#endif