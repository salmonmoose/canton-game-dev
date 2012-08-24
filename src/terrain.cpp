#include <iostream>
#include <list>

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

	pugi::xml_parse_result result = terrainConfig.load_file("basicTerrain.xml");

	terrainData = terrainConfig.child("document").child("terrain");

	//TODO: these dimensions should be expanded, and a chunking system introduced.
	int material;
	tc.values.resize(boost::extents[x_chunk][y_chunk][z_chunk]);
	tc.materials.resize(boost::extents[x_chunk][y_chunk][z_chunk]);

	setupAccidentalNoise();
}

void ScalarTerrain::setupAccidentalNoise() {
	std::vector<anl::CImplicitModuleBase *> layers;
	anl::CImplicitModuleBase * tmp;


    //anl::CImplicitSphere thisLayer;

    try {
    	pugi::xml_node layer = terrainData.child("layer");

    	if(strcmp(layer.attribute("type").value(), "sphere") == 0) {
            tmp = new anl::CImplicitSphere();

            dynamic_cast<anl::CImplicitSphere*>(tmp)->setCenter(
            	layer.child("center").attribute("x").as_double(),
            	layer.child("center").attribute("y").as_double(),
            	layer.child("center").attribute("z").as_double(),
            	layer.child("center").attribute("u").as_double(),
            	layer.child("center").attribute("v").as_double(),
            	layer.child("center").attribute("w").as_double()
            );

            layers.push_back(tmp);
        }
         else {
        	printf("Layer type not found\n");
        }


    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
    try {

		for(int z = 0; z < z_chunk; z++) {
			for(int y = 0; y < y_chunk; y++) {
				for(int x = 0; x < x_chunk; x++) {
					value = layers.back()->get(
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
	} catch (char * exception) {
		printf("Exception raised: %s\n", exception);
	}
}