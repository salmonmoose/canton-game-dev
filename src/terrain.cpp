#include <iostream>
#include <list>

#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "anl.h"

#include "canton.h"
#include "terrain.h"


static int x_chunk = 32;
static int y_chunk = 32;
static int z_chunk = 32;

using namespace irr;

ScalarTerrain::ScalarTerrain()
{
	printf("Loading Terrain Document\n");

    noiseTree.loadFile("basicTerrain.xml");

    printf("Terrain Loaded");
}

/*
Not sure exactly how this will work
-pass the terrain frustum 
-????
-get back mesh
-PROFIT!!!
*/
irr::scene::SMesh * ScalarTerrain::getMesh(/*frustum*/)
{
    worldMap[TerrainLocation(0,0,0)] = TerrainChunk();
    renderChunk(worldMap[TerrainLocation(0,0,0)]);
    worldMap[TerrainLocation(0,0,0)].Mesh = MCubeMesh::generateIsoSurface(tc);

    return worldMap[TerrainLocation(0,0,0)].Mesh;
}

void ScalarTerrain::generateNavMesh()
{

}
/**
Using noise tree, populates an array with values
**/
void ScalarTerrain::renderChunk(TerrainChunk &tc) {
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