#include "player.h"

Player::Player()
{
	playerMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
    playerEngine = IRR.smgr->addParticleSystemSceneNode(false);

    //TODO: this AABBox should come from engine meshes.
    playerEngineEmitter = playerEngine->createBoxEmitter(
                core::aabbox3d<f32>(-1,-0.25,-1,-0.75,0,-1),
                core::vector3df(0.0f,0.0f,0.0f),
                80,100, 
                video::SColor(0,255,255,255),
                video::SColor(0,255,255,255), 
                800,2000);
    
    playerEngineEmitter->setMinStartSize(core::dimension2d<f32>(0.25f, 0.25f));
    playerEngineEmitter->setMaxStartSize(core::dimension2d<f32>(0.25f, 0.25f));

    playerEngine->setEmitter(playerEngineEmitter);
    playerEngineEmitter->drop();

	//playerMesh->setMaterialFlag(EMF_GOURAUD_SHADING,false);

	Position.Y = 32.f;
    Speed = 64.f;
    Strafe = 32.f;
    Steering = 90.f;
    Drag = 0.5f;

}

Player::~Player()
{

}

void Player::Update()
{
    if(IRR.receiver.IsKeyDown(irr::KEY_KEY_A))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(-1,0,0), Rotation) * Strafe * IRR.frameDeltaTime;
    }
    else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_D))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(1,0,0), Rotation) * Strafe * IRR.frameDeltaTime;
    }

    if(IRR.receiver.IsKeyDown(irr::KEY_COMMA))
    {
        Rotation.Y -= Steering * IRR.frameDeltaTime;
    }
    else if(IRR.receiver.IsKeyDown(irr::KEY_PERIOD))
    {
        Rotation.Y += Steering * IRR.frameDeltaTime;
    }

	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_W))
	{
        Velocity += IRR.getRotatedVector(core::vector3df(0,0,1), Rotation) * Speed * IRR.frameDeltaTime;
	}
	else if(IRR.receiver.IsKeyDown(irr::KEY_KEY_S))
	{
		Velocity += IRR.getRotatedVector(core::vector3df(0,0,-1), Rotation) * Speed * IRR.frameDeltaTime;
	}

    Velocity -= Velocity * Drag * IRR.frameDeltaTime;

    if(Velocity.getLength() > Speed) Velocity.setLength(Speed);

    Position += IRR.frameDeltaTime * Velocity;
    
    playerEngine->setPosition(Position);
	playerMesh->setPosition(Position);
	playerMesh->setRotation(Rotation);
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