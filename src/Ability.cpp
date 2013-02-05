#include "Ability.h"

Ability::Ability()
{

}

void Ability::Initialize(Mob * mob)
{
    mMob = mob;
    Rate = 2000;
    LastTick = -Rate;
}

void Ability::Update()
{

}

void Ability::Trigger()
{

}

bool Ability::Ready()
{
	return IRR.GetGameTick() > LastTick + Rate;
}

const int Ability::GetCoolDown()
{
	if(!Ready()) 
	{
		return IRR.GetGameTick() - (LastTick + Rate);
	}
	else
	{
		return 0;
	}
}