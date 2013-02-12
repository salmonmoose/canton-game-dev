#include "FactoryAbilityFactory.h"

#include "AbilityFirePewPew.h"
#include "AbilityMovement.h"

void AbilityFactory::Initialize()
{
	RegisterTypes();
}

void AbilityFactory::RegisterTypes()
{
	printf("Registering Ability Types\n");

	bool FirePewPew_r = instance().register_type<FirePewPew>("FirePewPew");
	
	bool MoveForwards_r = instance().register_type<MoveForwards>("MoveForwards");
	bool MoveBackwards_r = instance().register_type<MoveBackwards>("MoveBackwards");
	bool StrafeLeft_r = instance().register_type<StrafeLeft>("StrafeLeft");
	bool StrafeRight_r = instance().register_type<StrafeRight>("StrafeRight");
}