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

    AddAbility("MoveForwards");
    AddAbility("MoveBackwards");
    AddAbility("StrafeLeft");
    AddAbility("StrafeRight");
    AddAbility("PointAtTarget");

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

const irr::core::vector3df & Player::getTargetPosition()
{
    return mouse->getPosition();
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

    TriggerAbility("PointAtTarget");

    if(IRR.receiver.KeyDown(irr::KEY_KEY_A))
    {
        TriggerAbility("StrafeLeft");
    }
    
    if(IRR.receiver.KeyDown(irr::KEY_KEY_D))
    {
        TriggerAbility("StrafeRight");
    }

    if(IRR.receiver.KeyDown(irr::KEY_KEY_W))
    {
        TriggerAbility("MoveForwards");
    }
    
    if(IRR.receiver.KeyDown(irr::KEY_KEY_S))
    {
        TriggerAbility("MoveBackwards");
    }
    
    if(IRR.receiver.ButtonDown(EventReceiver::MOUSE_BUTTON_LEFT))
    {
        TriggerAbility("FirePewPew");
    }
}