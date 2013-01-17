#ifndef ENEMY_H
#define ENEMY_H

#include "irrlicht.h"
#include "mob.h"

class Enemy : public Mob
{
public:
	Enemy(irr::core::vector3df pos, irr::core::vector3df rot);
	~Enemy();

	void Update();
	void Init();

    bool SeekTarget();
    void PointAtTarget();
    void Accelerate();

    float DistanceToTarget();

    std::shared_ptr<Mob> Target;

	double life;
};
#endif