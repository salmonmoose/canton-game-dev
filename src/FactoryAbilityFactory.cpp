#include "FactoryAbilityFactory.h"

#include "AbilityFirePewPew.h"

void AbilityFactory::Initialize()
{
	RegisterTypes();
}

void AbilityFactory::RegisterTypes()
{
	printf("Registering Ability Types\n");

	bool FirePewPew_r = instance().register_type<FirePewPew>("FirePewPew");
}