#include "player.h"

Player::Player()
{
	playerMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));

	//playerMesh->setMaterialFlag(EMF_GOURAUD_SHADING,false);

	Position.Y = 32.f;
    Speed = 64.f;
    Steering = 90.f;
    Drag = 0.1f;

}

Player::~Player()
{

}

void Player::Update()
{
    if(IRR.receiver.IsKeyDown(irr::KEY_KEY_A))
    {
        Rotation.Y -= Steering * IRR.frameDeltaTime;
    }
    else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_D))
    {
        Rotation.Y += Steering * IRR.frameDeltaTime;
    }

	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_W))
	{
        Velocity += getIrrIn() * Speed * IRR.frameDeltaTime;
	}
	else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_S))
	{
		Velocity -= getIrrIn() * Speed * IRR.frameDeltaTime;
	}

    Velocity -= Velocity * Drag * IRR.frameDeltaTime;

    if(Velocity.getLength() > Speed) Velocity.setLength(Speed);

    Position += IRR.frameDeltaTime * Velocity;

	playerMesh->setPosition(Position);
	playerMesh->setRotation(Rotation);
}

irr::core::vector3df Player::getIrrIn()
{
    irr::core::vector3df dir (core::vector3df(0,0,1));

    Matrix.setRotationDegrees(Rotation);
    Matrix.rotateVect(dir);

    return dir;
}

const irr::core::vector3df & Player::getPosition()
{
	return Position;
}

const irr::core::vector3df & Player::getRotation()
{
    return Rotation;
}

const irr::core::vector3df & Player::getVelocity()
{
    return Velocity;
}