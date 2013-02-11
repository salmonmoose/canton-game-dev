#ifndef MOBFACTORY_H
#define MOBFACTORY_H
#include "Factory.h"
#include "Mob.h"

class Mob;

struct MobFactory : Factory<std::string, Mob>
{
    static MobFactory & instance()
    {
    	static MobFactory sInstance;
    	return sInstance;
    }

    void RegisterTypes();
    void Initialize();
};

#endif