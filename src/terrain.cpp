#include <iostream>
#include <irrlicht.h>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "anl.h"

#include "canton.h"
#include "terrain.h"


static int x_chunk = 24;
static int y_chunk = 24;
static int z_chunk = 24;

using namespace irr;

ScalarTerrain::ScalarTerrain()
{
	tc = TerrainChunk();

	printf("Loading Terrain Document\n");
	terrainConfig.LoadFile("basicTerrain.xml");

	terrainData = terrainConfig.FirstChildElement("document")->FirstChildElement("terrain");

	//TODO: these dimensions should be expanded, and a chunking system introduced.
	int material;
	printf("Resizing node\n");
	tc.values.resize(boost::extents[x_chunk][y_chunk][z_chunk]);
	tc.materials.resize(boost::extents[x_chunk][y_chunk][z_chunk]);

	printf("Setting up accidental noise functions\n");
	setupAccidentalNoise();
}

void ScalarTerrain::setupAccidentalNoise() {
    anl::CImplicitCache result;
    anl::CImplicitSphere sphereGradient;

    try {
    	tinyxml2::XMLElement * nextElement = terrainData->FirstChildElement("layer");

        if(nextElement->Attribute("type", "sphere")) {
            sphereGradient.setCenter(
                nextElement->FirstChildElement("center")->DoubleAttribute("x"),
                nextElement->FirstChildElement("center")->DoubleAttribute("y"),
                nextElement->FirstChildElement("center")->DoubleAttribute("z"),
                nextElement->FirstChildElement("center")->DoubleAttribute("u"),
                nextElement->FirstChildElement("center")->DoubleAttribute("v"),
                nextElement->FirstChildElement("center")->DoubleAttribute("w")
            );

            //result.setSource(&sphereGradient);
        } else {
            printf("Didn't match\n");
        }
    } catch (char * except) {
        printf("Exception raised: %s\n", except);
    }

	for(int z = 0; z < z_chunk; z++) {
		for(int y = 0; y < y_chunk; y++) {
			for(int x = 0; x < x_chunk; x++) {
				value = sphereGradient.get(
                    (double) x/x_chunk * 2, 
                    (double) y/y_chunk * 2, 
                    (double) z/z_chunk * 2
                );

				tc.values[x][y][z] = value;

				if(value < -0.5) tc.materials[x][y][z] = 0;
				else if (value < 0) tc.materials[x][y][z] = 1;
				else if (value < 0.5) tc.materials[x][y][z] = 2;
				else tc.materials[x][y][z] = 3;
			}
		}
	}
}