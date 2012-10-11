#include "player.h"

Player::Player()
{
	playerMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));

	//playerMesh->setMaterialFlag(EMF_GOURAUD_SHADING,false);

	Position.Y = 32.f;
    Speed = 0.3f;
}

Player::~Player()
{

}

void Player::Update()
{

	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_W))
	{
        Position += getIrrIn() * Speed * IRR.frameDeltaTime;
	}
	else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_S))
	{
		Position -= getIrrIn() * Speed * IRR.frameDeltaTime;
	}

	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_A))
	{
		Rotation.Y -= 3.f * IRR.frameDeltaTime;
	}
	else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_D))
	{
		Rotation.Y += 3.f * IRR.frameDeltaTime;
	}

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
