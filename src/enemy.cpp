#include "enemy.h"

Enemy::Enemy(irr::core::vector3df pos, irr::core::vector3df rot)
{
	Position = pos;
	Rotation = rot;

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