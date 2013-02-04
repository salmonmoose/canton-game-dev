#ifndef ENEMY_H
#define ENEMY_H

#include "irrlicht.h"
#include "Mob.h"

class Enemy : public Mob
{
public:
	Enemy();
	~Enemy();

	void Update();
	void Init();

    bool SeekTarget();
    void PointAtTarget();
    void Accelerate();
    void Deccelerate();

    float DistanceToTarget();

    std::shared_ptr<Mob> Target;

	double life;
};
#endif