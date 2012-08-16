#include <iostream>

class ScalarTerrain
{
private:
	double value;

public:
	TerrainChunk tc;

	ScalarTerrain();

	~ScalarTerrain(){};

	void setupAccidentalNoise();
	void setupNoise();
	void trashNoise();
	void generateChunk(int, int, int);
};