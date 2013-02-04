#include "MobPewPew.h"
#include "State.h"
#include "Engine.h"

class PewPewFiredState : public State
{
    PewPew * mPewPew;
public:
    PewPewFiredState(PewPew * pewpew)
    {
        mPewPew = pewpew;
    };
    ~PewPewFiredState(){};

    virtual void OnUpdate()
    {
        mPewPew->Accelerate();
        mPewPew->ApplyVectors();
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


PewPew::PewPew() : Mob(true, true, true)
{
}

void PewPew::Init()
{
    mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
    life = 100.f;
    MaxSpeed = 80.f;
    MaxStrafe = 0.f;
    MaxTurnRate = 45.f;
    Drag = 0.1f;

    SetState(new PewPewFiredState(this));
}

void PewPew::Update()
{
    Mob::Update();
}

void PewPew::Accelerate()
{
    Velocity += IRR.getRotatedVector(core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;   
}