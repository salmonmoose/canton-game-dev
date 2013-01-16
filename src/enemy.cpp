#include "enemy.h"
#include "state.h"
#include "engine.h"

class EnemyIdleState : public State
{
	Enemy * mEnemy;

public:
	EnemyIdleState(Enemy * enemy)
	{
		mEnemy = enemy;
	};
	
	~EnemyIdleState(){};

	virtual void OnUpdate()
	{
		mEnemy->ApplyVectors();
	}

	virtual void OnEnter()
	{

	}

	virtual void OnLeave()
	{

	}

	virtual void OnMessage(std::string * message)
	{

	}
};

Enemy::Enemy(irr::core::vector3df pos, irr::core::vector3df rot) : Mob(true,true,true)
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
	Velocity = irr::core::vector3df(0.f,0.f,0.f);
	SetState(new EnemyIdleState(this));
}

void Enemy::Update()
{
	Mob::Update();
}