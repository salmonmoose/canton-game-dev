#ifndef ENEMY_H
#define ENEMY_H

#include "irrlicht.h"
#include "engine.h"
#include "mob.h"
#include "shadercallback.h"

class Enemy : public Mob
{
public:
	Enemy(irr::core::vector3df pos, irr::core::vector3df rot);
	~Enemy();

	void Update();
	void Init();

	double life;
};
#endif