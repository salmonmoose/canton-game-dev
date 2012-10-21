#include "mob.h"

void Mob::Update()
{
    Velocity -= Velocity * Drag * IRR.frameDeltaTime;

    if(Velocity.getLength() > MaxSpeed) Velocity.setLength(MaxSpeed);

    Position += IRR.frameDeltaTime * Velocity;

    mainMesh->setPosition(Position);
    mainMesh->setRotation(Rotation);
}

void Mob::Init()
{

}