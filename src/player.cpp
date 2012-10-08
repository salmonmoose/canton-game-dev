#include "player.h"

Player::Player()
{
	playerMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));

	Position.Y = 24.f;
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
         Matrix.setRotationDegrees(Rotation);
         Matrix.setTranslation(Position);
         Matrix.setScale(core::vector3df(1.f,1.f,1.f));

         vector3df in(Matrix[8],Matrix[9],Matrix[10]); 
         in.normalize(); 
         return in;
}

const irr::core::vector3df & Player::getPosition()
{
	return Position;
}