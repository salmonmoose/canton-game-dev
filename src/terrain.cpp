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

    noiseTree.loadFile("basicTerrain.xml");

    printf("Terrain Loaded");

	//TODO: these dimensions should be expanded, and a chunking system introduced.
	int material;
	tc.values.resize(boost::extents[x_chunk][y_chunk][z_chunk]);
	tc.materials.resize(boost::extents[x_chunk][y_chunk][z_chunk]);

    renderChunk();
}

void ScalarTerrain::renderChunk() {
    try {
        for(int z = 0; z < z_chunk; z++) {
            for(int y = 0; y < y_chunk; y++) {
                for(int x = 0; x < x_chunk; x++) {
                    value = noiseTree.get(
                        (double) x/x_chunk * 2, 
                        (double) y/y_chunk * 2, 
                        (double) z/z_chunk * 2
                    );

                    tc.values[x][y][z] = value;

                    //printf("value at position (%d,%d,%d) is %f\n",x,y,z,value);

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