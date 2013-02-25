#include "FactoryMobFactory.h"

#include "MobPlayer.h"
#include "MobEnemy.h"
#include "MobPewPew.h"

void MobFactory::Initialize()
{
	RegisterTypes();
}

void MobFactory::RegisterTypes()
{
	printf("Registering Mob Types\n");
	bool Player_r = instance().register_type<Player>("Player");
	bool Enemy_r = instance().register_type<Enemy>("Enemy");
	bool PewPew_r = instance().register_type<PewPew>("PewPew");

	assert(Player_r);
	assert(Enemy_r);
	assert(PewPew_r);
}