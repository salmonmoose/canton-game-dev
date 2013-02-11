#ifndef ABILITYFACTORY_H
#define ABILITYFACTORY_H
#include "Factory.h"
#include "Ability.h"

class Ability;

struct AbilityFactory : Factory<std::string, Ability>
{
	static AbilityFactory & instance()
	{
		static AbilityFactory sInstance;
		return sInstance;
	}

	void RegisterTypes();
	void Initialize();
};

#endif