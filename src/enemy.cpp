#include "enemy.h"
#include "engine.h"

Enemy::Enemy(irr::core::vector3df pos, irr::core::vector3df rot) : Mob(true,true,true)
{
	Position = pos;
	Rotation = rot;

	printf("Position (%f,%f,%f)\n", Position.X, Position.Y, Position.Z);

	mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
}

Enemy::~Enemy()
{
	
}

void Enemy::Init()
{
	life = 100.f;
	MaxSpeed = 64.f;
	MaxStrafe = 32.f;
	MaxTurnRate = 90.f;
	Drag = 0.5f;
}

void Enemy::Update()
{
	Mob::Update();
}