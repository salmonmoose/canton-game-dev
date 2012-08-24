#include <iostream>
#include "tinyxml2.h"
#include "pugixml.hpp"

#include "anl.h"
#include <irrlicht.h>

class ScalarTerrain
{
private:
	double value;

public:
    TerrainChunk tc;
	//tinyxml2::XMLDocument terrainConfig;
	//tinyxml2::XMLElement * terrainData;

	pugi::xml_document terrainConfig;

	pugi::xml_node terrainData;

	ScalarTerrain();

	~ScalarTerrain(){};

	void setupAccidentalNoise();
	void generateChunk(int, int, int);
	void bresenham(irr::core::vector3df, irr::core::vector3df);
};