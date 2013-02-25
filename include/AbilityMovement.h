#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "Ability.h"

class MoveForwards : public Ability
{
public:
	MoveForwards();
	void Trigger();
};

class MoveBackwards : public Ability
{
public:
	MoveBackwards();
	void Trigger();
};

class StrafeLeft : public Ability
{
public:
	StrafeLeft();
	void Trigger();
};

class StrafeRight : public Ability
{
public:
	StrafeRight();
	void Trigger();
};

class PointAtTarget : public Ability
{
public:
	PointAtTarget();
	void Trigger();
};

#endif

