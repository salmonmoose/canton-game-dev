#include "player.h"

Player::Player()
{
	mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
    playerEngine1 = IRR.smgr->addParticleSystemSceneNode(false);
    playerEngine2 = IRR.smgr->addParticleSystemSceneNode(false);

    playerEngine1->setParent(mainMesh);
    playerEngine2->setParent(mainMesh);

    //TODO: this AABBox should come from engine meshes.
    playerEngineEmitter1 = playerEngine1->createBoxEmitter(
                core::aabbox3d<f32>(-0.5,-0.25,-1,-0.25,0,-1),
                core::vector3df(0.0f,0.0f,-0.0f),
                50,60, 
                video::SColor(0,255,255,255),
                video::SColor(0,255,255,255), 
                100,200);
    
    playerEngineEmitter2 = playerEngine2->createBoxEmitter(
                core::aabbox3d<f32>(0.5,-0.25,-1,0.25,0,-1),
                core::vector3df(0.0f,0.0f,-0.0f),
                50,60,
                video::SColor(0,255,255,255),
                video::SColor(0,255,255,255),
                100,200);

    playerEngineEmitter1->setMinStartSize(core::dimension2d<f32>(0.5f, 0.5f));
    playerEngineEmitter1->setMaxStartSize(core::dimension2d<f32>(0.5f, 0.5f));

    playerEngineEmitter2->setMinStartSize(core::dimension2d<f32>(0.5f, 0.5f));
    playerEngineEmitter2->setMaxStartSize(core::dimension2d<f32>(0.5f, 0.5f));

    playerEngine1->setEmitter(playerEngineEmitter1);
    playerEngine2->setEmitter(playerEngineEmitter2);
    playerEngineEmitter1->drop();
    playerEngineEmitter2->drop();

    //playerEngine1->setMaterialFlag(video::EMF_LIGHTING, false);
    //playerEngine2->setMaterialFlag(video::EMF_LIGHTING, false);

    //playerEngine1->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
    //playerEngine2->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);

    playerEngine1->setMaterialTexture(0, IRR.driver->getTexture("./resources/fireball.bmp"));
    playerEngine2->setMaterialTexture(0, IRR.driver->getTexture("./resources/fireball.bmp"));
    //playerEngine1->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
    //playerEngine2->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

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
    if(IRR.receiver.KeyDown(irr::KEY_KEY_A))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(-1,0,0), Rotation) * MaxStrafe * IRR.frameDeltaTime;
    }
    
    if(IRR.receiver.KeyDown(irr::KEY_KEY_D))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(1,0,0), Rotation) * MaxStrafe * IRR.frameDeltaTime;
    }

    if(IRR.receiver.KeyDown(irr::KEY_COMMA))
    {
        Rotation.Y -= MaxTurnRate * IRR.frameDeltaTime;
    }
    if(IRR.receiver.KeyDown(irr::KEY_PERIOD))
    {
        Rotation.Y += MaxTurnRate * IRR.frameDeltaTime;
    }

	if(IRR.receiver.KeyDown(irr::KEY_KEY_W))
	{
        Velocity += IRR.getRotatedVector(core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
	}
	
    if(IRR.receiver.KeyDown(irr::KEY_KEY_S))
	{
		Velocity += IRR.getRotatedVector(core::vector3df(0,0,-1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
	}

    if(IRR.receiver.ButtonPressed((EventReceiver::MOUSE_BUTTON_ENUM) 0))
    {
        std::unique_ptr<Mob> missile = std::unique_ptr<Mob>(new PewPew(Position, Rotation, Velocity));

        missile->Init();

        IRR.mobL.push_back(std::move(missile));
    }

    Mob::Update();
}