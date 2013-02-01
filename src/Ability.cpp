#include "Ability.h"

void Ability::Initialize(Mob * mob)
{
    mMob = mob;
    Rate = 2000;
    LastTick = -Rate;
}

void Ability::Update()
{

}

void Ability::Activate()
{
    if(IRR.GetGameTick() > LastTick + Rate)
    {
        LastTick = IRR.GetGameTick();
    }
}