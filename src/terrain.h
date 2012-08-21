#include <iostream>
#include "tinyxml2.h"
#include "anl.h"


class ScalarTerrain
{
private:
	double value;

public:
    TerrainChunk tc;
	tinyxml2::XMLDocument terrainConfig;
	tinyxml2::XMLElement * terrainData;

	ScalarTerrain();

	~ScalarTerrain(){};

	void setupAccidentalNoise();
	void generateChunk(int, int, int);
};