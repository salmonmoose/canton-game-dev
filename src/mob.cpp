#include "mob.h"

Mob::Mob()
{
    mState = new NullState();
}

void Mob::Update()
{
	mState->OnUpdate(); //AI Hook
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