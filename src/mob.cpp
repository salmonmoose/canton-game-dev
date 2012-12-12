#include "mob.h"
#include "nullstate.h"
#include "engine.h"

Mob::Mob(bool Visible, bool Solid, bool Mobile) : _Visible(Visible), _Solid(Solid), _Mobile(Mobile)
{
    mState = new NullState();
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