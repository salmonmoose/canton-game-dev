#ifndef PEWPEW_H
#define PEWPEW_H

#include "irrlicht.h"
#include "Mob.h"

class PewPew : public Mob
{
public:
	irr::scene::IParticleSystemSceneNode * particles;
	irr::scene::IParticleEmitter * emitter;
    irr::io::path vsFileName;
    irr::io::path psFileName;
    irr::video::SMaterial Material;
    irr::s32 MatID;
	PewPew();
	~PewPew(){};
	void Update();
    void Init();
    void Accelerate();
	double life;
};
#endif