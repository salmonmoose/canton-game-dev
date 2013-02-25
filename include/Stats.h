#ifndef STATS_H
#define STATS_H

class Stats
{
private:
	int _pilot;
	int _engine;
	int _computer;
	int _generator;

public:

	Stats()
	{
		_pilot = 10;
		_engine = 10;
		_computer = 10;
		_generator = 10;
	}

	const int GetPilot(){ return _pilot; }
	const int GetEngine(){ return _engine; }
	const int GetComputer(){ return _computer; }
	const int GetGenerator(){ return _generator; }

	void SetPilot(int value){ _pilot = value; }
	void SetEngine(int value){ _engine = value; }
	void SetComputer(int value){ _computer = value; }
	void SetGenerator(int value){ _generator = value; }

	const int GetThrust() 
	{
		return 4 * _engine;
	}

	const int GetStrafe()
	{
		return 2 * _engine;
	}

	const int GetTurnRate()
	{
		return 8 * _pilot;
	}
};

#endif