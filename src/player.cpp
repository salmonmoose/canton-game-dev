#include "player.h"

Player::Player()
{
	playerMesh = IRR.smgr->addCubeSceneNode();
	Position.Y = 8.f;
}

Player::~Player()
{

}

void Player::Update()
{
	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_W))
	{
		Position.Z += 0.3f * IRR.frameDeltaTime;
	}
	else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_S))
	{
		Position.Z -= 0.3f * IRR.frameDeltaTime;
	}

	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_A))
	{
		Rotation.Y -= 0.3f * IRR.frameDeltaTime;
	}
	else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_D))
	{
		Rotation.Y += 0.3f * IRR.frameDeltaTime;
	}

	playerMesh->setPosition(Position);
	playerMesh->setRotation(Rotation);
}

const irr::core::vector3df & Player::getPosition()
{
	return Position;
}