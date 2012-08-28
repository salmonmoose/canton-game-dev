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
    
    anl::CImplicitXML noiseTree;

	ScalarTerrain();

	~ScalarTerrain(){};

	void setupAccidentalNoise();
	void renderChunk();
	void generateChunk(int, int, int);
	void bresenham(irr::core::vector3df, irr::core::vector3df);
};