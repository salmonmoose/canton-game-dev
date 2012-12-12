#ifndef NULLSTATE_H
#define NULLSTATE_H

#include "state.h"

class NullState : public State
{
public:
    NullState();

    virtual ~NullState();

    virtual void OnUpdate();

    virtual void OnEnter();

    virtual void OnLeave();

    virtual void OnMessage(std::string * message);
};

#endif