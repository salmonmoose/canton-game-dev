
#include "pewpew.h"
#include "state.h"
#include "engine.h"

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


PewPew::PewPew(irr::core::vector3df pos, irr::core::vector3df rot, irr::core::vector3df vel) : Mob(true, true, true)
{
    Position = pos;
    Rotation = rot;
    Velocity = vel;
    mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));
}

void PewPew::Init()
{
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