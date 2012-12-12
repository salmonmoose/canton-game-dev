#ifndef STATE_H
#define STATE_H

#include "irrlicht.h"
#include "mob.h"
#include <iostream>

class State
{
public:
    State(){};

    virtual ~State(){};

	virtual void OnEnter(){};
	virtual void OnLeave(){};
	virtual void OnUpdate(){};
	virtual void OnMessage(std::string * message){};
};

#endif