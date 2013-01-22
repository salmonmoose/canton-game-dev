#include "enemy.h"
#include "state.h"
#include "engine.h"

class EnemyState : public State
{
protected:
    Enemy * mEnemy;
public:
    EnemyState(Enemy * enemy)
    {
        mEnemy = enemy;
    }

    ~EnemyState(){}
};

class EnemyHuntState : public EnemyState
{
public:
    EnemyHuntState(Enemy * enemy) : EnemyState(enemy) {}

    virtual void OnUpdate()
    {
        mEnemy->PointAtTarget();
        if(mEnemy->DistanceToTarget() > 20)
            mEnemy->Accelerate();

        if(mEnemy->DistanceToTarget() < 10)
            mEnemy->Deccelerate();
        mEnemy->ApplyVectors();
    }

    virtual void OnEnter(){}

    virtual void OnLeave(){}

    virtual void OnMessage(std::string * message){}
};

class EnemyIdleState : public EnemyState
{
public:
    EnemyIdleState(Enemy * enemy) : EnemyState(enemy) {}

	virtual void OnUpdate()
	{
        if(mEnemy->SeekTarget())
        {
            mEnemy->SetState(new EnemyHuntState(mEnemy));
        }
        //if target, change state
		mEnemy->ApplyVectors();
	}

	virtual void OnEnter(){}

	virtual void OnLeave(){}

	virtual void OnMessage(std::string * message){}
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
	MaxSpeed = 96.f;
	MaxStrafe = 32.f;
	MaxTurnRate = 240.f;
	Drag = 0.5f;
	Velocity = irr::core::vector3df(0.f,0.f,0.f);
	SetState(new EnemyIdleState(this));
}

void Enemy::Update()
{
	Mob::Update();
}

bool Enemy::SeekTarget()
{
    if(Position.getDistanceFrom(IRR.mPlayer->getPosition()) < 20)
    {
        Target = IRR.mPlayer;
        return true;
    }
    else
    {
        return false;
    }
}

void Enemy::PointAtTarget()
{
    float angle = getAngleToVector(Target->getPosition());

    Rotation.Y += MaxTurnRate * (angle / 1.f) * IRR.frameDeltaTime;

    if(Rotation.Y < -360)
        Rotation.Y += 360;
    if(Rotation.Y > 360)
        Rotation.Y -= 360;
}

void Enemy::Accelerate()
{
    Velocity += IRR.getRotatedVector(irr::core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
}

void Enemy::Deccelerate()
{
    Velocity -= IRR.getRotatedVector(irr::core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
}

float Enemy::DistanceToTarget()
{
    return Position.getDistanceFrom(Target->getPosition());
}