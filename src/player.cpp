#include "player.h"

Player::Player()
{
	mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
    playerEngine = IRR.smgr->addParticleSystemSceneNode(false);

    playerEngine->setParent(mainMesh);

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
    MaxSpeed = 64.f;
    MaxStrafe = 32.f;
    MaxTurnRate = 90.f;
    Drag = 0.5f;

}

Player::~Player()
{

}

void Player::Update()
{
    if(IRR.receiver.IsKeyDown(irr::KEY_KEY_A))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(-1,0,0), Rotation) * MaxStrafe * IRR.frameDeltaTime;
    }
    
    if(IRR.receiver.IsKeyDown(irr::KEY_KEY_D))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(1,0,0), Rotation) * MaxStrafe * IRR.frameDeltaTime;
    }

    if(IRR.receiver.IsKeyDown(irr::KEY_COMMA))
    {
        Rotation.Y -= MaxTurnRate * IRR.frameDeltaTime;
    }
    if(IRR.receiver.IsKeyDown(irr::KEY_PERIOD))
    {
        Rotation.Y += MaxTurnRate * IRR.frameDeltaTime;
    }

	if(IRR.receiver.IsKeyDown(irr::KEY_KEY_W))
	{
        Velocity += IRR.getRotatedVector(core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
	}
	
    if(IRR.receiver.IsKeyDown(irr::KEY_KEY_S))
	{
		Velocity += IRR.getRotatedVector(core::vector3df(0,0,-1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
	}

    Mob::Update();
}