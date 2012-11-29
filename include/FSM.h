//FSM.h
#ifndef FSM_H
#define FSM_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

class FSM;

class FSMState
{
public:
	FSMState(){};
	FSMState(FSM *fsm){};
	virtual ~FSMState(){};
	virtual void Update(const float& dt) = 0;
	virtual void DoENTER(){};
	virtual void DoEXIT(){};

	std::string stateName;
	FSM *fsm;
};

typedef std::vector<std::tr1::shared_ptr<FSMState>> StateBank;
typedef std::vector<std::tr1::shared_ptr<FSMState>>::iterator StateBankIterator;

class FSM
{
public:
	FSM();
	~FSM();
	void Update(const float& dt);

	void TransitionTo(std::string stateName);
	void DelayTransitionTo(std::string stateName);
	void AddState(FSMState * newState, bool makeCurrent);
	std::string GetState();

public:
	FSMState * currentState;
	std::string delayState;

	StateBank stateBank;
	StateBankIterator iter;
};
#endif