#include "Mob.h"
#include "StateNullState.h"
#include "Engine.h"
#include "FactoryAbilityFactory.h"

Mob::Mob(bool Visible, bool Solid, bool Mobile) : _Visible(Visible), _Solid(Solid), _Mobile(Mobile)
{
    mState = new NullState();
    mapAbility = new std::map<std::string, std::shared_ptr<Ability>>();
}

void Mob::AddAbility(std::string ID)
{
    std::shared_ptr<Ability> incomming = AbilityFactory::instance().build_shared_ptr(ID);

    if(incomming)
    {
        incomming->Initialize(this);
        (*mapAbility)[ID] = incomming;
    }
}

void Mob::TriggerAbility(std::string ID)
{
    auto fit = mapAbility->find(ID);

    if (fit == mapAbility->end())
    {
        printf("Didn't find: %s\n", ID.c_str());
        return;
    }

    fit->second->Trigger();
}

void Mob::Update()
{
    if(_Mobile)
    {
	   mState->OnUpdate(); //AI Hook
    }
}

void Mob::Draw()
{
    if(_Visible)
    {
        ///????
    }
}

void Mob::Collide(Mob mobs[])
{
    if(_Solid)
    {
        ///????
    }
}

void Mob::Init()
{

}

void Mob::SetState(State * newState)
{
	if(mState) 
    {
        mState->OnLeave();
        delete mState;
    }
	mState = newState;

	mState->OnEnter();
}

void Mob::OnMessage(std::string * message)
{
	mState->OnMessage(message);
}

void Mob::ApplyVectors()
{
    Velocity -= Velocity * Drag * IRR.frameDeltaTime;

    if(Velocity.getLength() > MaxSpeed) Velocity.setLength(MaxSpeed);

    Position += IRR.frameDeltaTime * Velocity;

    mainMesh->setPosition(Position);
    mainMesh->setRotation(Rotation);
}

const float Mob::getAngleToVector(irr::core::vector3df target)
{
    irr::core::vector3df mobDirection = IRR.getRotatedVector(irr::core::vector3df(0,0,1), Rotation);
    irr::core::vector3df targetDirection = target - Position;

    mobDirection.normalize();
    targetDirection.normalize();

    irr::core::vector3df targetNormal(-targetDirection.Z, 1, targetDirection.X);

    if(mobDirection.dotProduct(targetNormal) > 0)
    {
        return acos(mobDirection.dotProduct(targetDirection));
    }
    else
    {
        return 0-acos(mobDirection.dotProduct(targetDirection));
    }
}

const float Mob::getDistanceToVector(irr::core::vector3df target)
{
    return Position.getDistanceFrom(target);
}