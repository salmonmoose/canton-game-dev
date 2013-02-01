#include "MobPlayer.h"
#include "State.h"
#include "Engine.h"
#include "MobPewPew.h"
#include "MobEnemy.h"

class PlayerControledState : public State
{
    Player * mPlayer;
public:
    PlayerControledState(Player * player)
    {
        mPlayer = player;
    };
    ~PlayerControledState(){};

    virtual void OnUpdate()
    {
        mPlayer->AcceptInput();
        mPlayer->ApplyVectors();
    }

    virtual void OnEnter()
    {

    }

    virtual void OnLeave()
    {

    }

    virtual void OnMessage(std::string * message)
    {

    }
};

Player::Player():Mob(true, true, true)
{

}

Player::~Player()
{

}

void Player::Init()
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
        100,200
    );
    
    playerEngineEmitter2 = playerEngine2->createBoxEmitter(
        core::aabbox3d<f32>(0.5,-0.25,-1,0.25,0,-1),
        core::vector3df(0.0f,0.0f,-0.0f),
        50,60,
        video::SColor(0,255,255,255),
        video::SColor(0,255,255,255),
        100,200
    );

    playerEngineEmitter1->setMinStartSize(core::dimension2d<f32>(0.5f, 0.5f));
    playerEngineEmitter1->setMaxStartSize(core::dimension2d<f32>(0.5f, 0.5f));

    playerEngineEmitter2->setMinStartSize(core::dimension2d<f32>(0.5f, 0.5f));
    playerEngineEmitter2->setMaxStartSize(core::dimension2d<f32>(0.5f, 0.5f));

    playerEngine1->setEmitter(playerEngineEmitter1);
    playerEngine2->setEmitter(playerEngineEmitter2);
    playerEngineEmitter1->drop();
    playerEngineEmitter2->drop();

    playerEngine1->setMaterialTexture(0, IRR.driver->getTexture("./resources/fireball.bmp"));
    playerEngine2->setMaterialTexture(0, IRR.driver->getTexture("./resources/fireball.bmp"));

    Position.Y = 32.f;
    MaxSpeed = 64.f;
    MaxStrafe = 32.f;
    MaxTurnRate = 90.f;
    Drag = 0.5f;
    Lift = 20.f;
    Gravity = 9.8f;
    TurnBuffer = 1;
    mouse = new Mouse();

    SetState(new PlayerControledState(this));
}

float Player::getAngleToMouse()
{
    irr::core::vector3df ship_dir = IRR.getRotatedVector(core::vector3df(0,0,1), Rotation); //Fix me: this works but is WRONG.
    irr::core::vector3df mouse_dir = mouse->getPosition() - Position;

    ship_dir.normalize();
    mouse_dir.normalize();

    float dot_product = ship_dir.dotProduct(mouse_dir);

    float angle = acos(dot_product);

    irr::core::vector3df mouse_normal (-mouse_dir.Z, 1, mouse_dir.X);

    dot_product = ship_dir.dotProduct(mouse_normal);

    if(dot_product > 0)
    {
        return angle;
    }
    else
    {
        return -angle;
    }
}

void Player::AcceptInput()
{
    mouse->Update(Position.Y);

    double ground_height = (double) IRR.mVoxelSceneNode->GetAltitude(Position);

    if(ground_height + 32.f > Position.Y)
    {
        //Velocity.Y += Lift * IRR.frameDeltaTime;        
    }

    //FIXME: this should be a property of all MOBs, Gravity should be per-level set.
    //Velocity.Y -= Gravity * IRR.frameDeltaTime;


    if(IRR.receiver.KeyDown(irr::KEY_KEY_A))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(-1,0,0), Rotation) * MaxStrafe * IRR.frameDeltaTime;
    }
    
    if(IRR.receiver.KeyDown(irr::KEY_KEY_D))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(1,0,0), Rotation) * MaxStrafe * IRR.frameDeltaTime;
    }

    float angle = getAngleToMouse();

    Rotation.Y += MaxTurnRate * (angle / TurnBuffer) * IRR.frameDeltaTime;
    if(Rotation.Y < -360)
        Rotation.Y += 360;
    if(Rotation.Y > 360)
        Rotation.Y -= 360;

    if(IRR.receiver.KeyDown(irr::KEY_KEY_W))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
    }
    
    if(IRR.receiver.KeyDown(irr::KEY_KEY_S))
    {
        Velocity += IRR.getRotatedVector(core::vector3df(0,0,-1), Rotation) * MaxSpeed * IRR.frameDeltaTime;
    }

    if(IRR.receiver.KeyDown(irr::KEY_SPACE))
    {
        //FIXME: This should be abstracted to the engine;
        std::shared_ptr<Mob> missile = std::shared_ptr<Mob>(
            new PewPew(Position, Rotation, Velocity)
            );

        missile->Init();

        IRR.AddMob(missile);
    }

    if(IRR.receiver.KeyDown(irr::KEY_LCONTROL))
    {
        std::shared_ptr<Mob> enemy = std::shared_ptr<Mob>(
            new Enemy(
                mouse->getPosition(), 
                Rotation)
            );
        enemy->Init();
        
        IRR.AddMob(enemy);
    }
/*
    if(IRR.receiver.ButtonPressed(EventReceiver::MOUSE_BUTTON_RIGHT))
    {
        //FIXME: This should be abstracted to the engine;
        std::unique_ptr<Mob> enemy = std::unique_ptr<Mob>(new Enemy(mouse->getPosition(), Rotation));

        enemy->Init();

        IRR.vMob->push_back(std::move(enemy));
    }
*/
    if(IRR.receiver.ButtonDown(EventReceiver::MOUSE_BUTTON_LEFT))
    {
        IRR.mVoxelSceneNode->AddBrush(mouse->getPosition());
    }

    if(IRR.receiver.ButtonDown(EventReceiver::MOUSE_BUTTON_RIGHT))
    {
        IRR.mVoxelSceneNode->RemoveBrush(mouse->getPosition());
    }
}