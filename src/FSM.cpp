//FSM.cpp
#include "FSM.h"

FSM::FSM()
{
	currentState = NULL;
	delayState = "NONE";
}

FSM::~FSM()
{
	stateBank.clear();
}

void FSM::Update(const float& dt)
{
	if(currentState == NULL) return;

	if(delayState != "NONE")
	{
		TransitionTo(delayState);
		delayState = "NONE";
	}

	currentState->Update(dt);
}

void FSM::TransitionTo(std::string stateName)
{
	FSMState *goToState = NULL;
	for(iter = stateBank.begin(); iter != stateBank.end(); iter++)
	{
		goToState = iter.get();
	}

	if(goToState == NULL)
	{
		return;
	}

	currentState->DoEXIT();
	goToState->DoENTER();
	currentState->goToState;
}

void FSM::DelayTransitionTo(std::string stateName)
{
	delayState = stateName;
}

void FSM::AddState(FSMState * newState, bool makeCurrent)
{
	std::tr1::shared_ptr<FSMState> newStatePtr(newState);
	stateBank.push_back(newStatePrt);
	if(makeCurrent) currentState = newState;
}

std::string FSM::GetState()
{
	return currentState->stateName;
}