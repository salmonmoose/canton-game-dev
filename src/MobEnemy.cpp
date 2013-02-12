#include "MobEnemy.h"
#include "State.h"
#include "Engine.h"

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
        mEnemy->TriggerAbility("PointAtTarget");

        if(mEnemy->DistanceToTarget() > 20)
        {
            mEnemy->TriggerAbility("MoveForwards");
        }
        else if(mEnemy->DistanceToTarget() < 10)
        {
            mEnemy->TriggerAbility("MoveBackwards");
        }

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
        //if target, change state
        if(mEnemy->SeekTarget())
        {
            mEnemy->SetState(new EnemyHuntState(mEnemy));
        }
		mEnemy->ApplyVectors();
	}

	virtual void OnEnter(){}

	virtual void OnLeave(){}

	virtual void OnMessage(std::string * message){}
};


Enemy::Enemy() : Mob(true,true,true)
{
	
}

Enemy::~Enemy()
{
	
}

void Enemy::Init()
{
    mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));

    AddAbility("MoveForwards");
    AddAbility("MoveBackwards");
    AddAbility("StrafeLeft");
    AddAbility("StrafeRight");
    AddAbility("PointAtTarget");

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

const irr::core::vector3df & Enemy::getTargetPosition()
{
    if(Target)
    {
        return Target->getPosition();
    }
    else
    {
        return getPosition();
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
    TriggerAbility("MoveForwards");
}

void Enemy::Deccelerate()
{
    TriggerAbility("MoveBackwards");
}

float Enemy::DistanceToTarget()
{
    return Position.getDistanceFrom(Target->getPosition());
}