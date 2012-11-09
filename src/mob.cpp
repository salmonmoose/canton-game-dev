#include "mob.h"

void Mob::Update()
{
	//mState->OnUpdate(); //AI Hook
    Velocity -= Velocity * Drag * IRR.frameDeltaTime;

    if(Velocity.getLength() > MaxSpeed) Velocity.setLength(MaxSpeed);

    Position += IRR.frameDeltaTime * Velocity;

    mainMesh->setPosition(Position);
    mainMesh->setRotation(Rotation);
}

void Mob::Init()
{

}

void Mob::SetState(State * newState)
{
	mState->OnLeave();
	delete mState;

	mState = newState;

	mState->OnEnter();
}

void Mob::OnMessage(std::string * message)
{
	mState->OnMessage(message);
}