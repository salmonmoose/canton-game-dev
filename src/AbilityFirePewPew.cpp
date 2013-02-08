#include "AbilityFirePewPew.h"

FirePewPew::FirePewPew() : Ability()
{

}

void FirePewPew::Trigger()
{
	if(Ready())
	{
		IRR.AddMob("PewPew");
		IRR.mMob->SetPosition(Parent()->getPosition());
		IRR.mMob->SetRotation(Parent()->getRotation());
		IRR.mMob->SetVelocity(Parent()->getVelocity());

		Ability::Trigger();
	}
}