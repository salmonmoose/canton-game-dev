#include "Ability.h"

Ability::Ability()
{

}

void Ability::Initialize(Mob * mob)
{
    mMob = mob;
    Rate = 200;
    LastTick = -Rate;
}

void Ability::Update()
{

}

void Ability::Trigger()
{
	LastTick = IRR.GetGameTick();
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