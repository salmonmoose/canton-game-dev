#include <iostream>
#include <list>

#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "anl.h"

#include "canton.h"
#include "terrain.h"


static int x_chunk = 16;
static int y_chunk = 16;
static int z_chunk = 16;

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
void ScalarTerrain::getMesh(/*frustum*/)
{
    //Loop through all chunks in frustum
    printf("Spawn Chunk\n");
    worldMap[TerrainLocation(0,0,0)] = TerrainChunk();
    printf("RenderChunk\n");
    renderChunk(worldMap[TerrainLocation(0,0,0)]);

    printf("Add mesh\n");

    printf("setting up buf\n");

    generateIsoSurface(
        Mesh,
        (* worldMap[TerrainLocation(0,0,0)].values),
        (* worldMap[TerrainLocation(0,0,0)].materials)
        );

    printf("buf setup\n");
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

                    (*tc.values)[x][y][z] = value;

                    if(value < -0.5) (*tc.materials)[x][y][z] = 0;
                    else if (value < 0) (*tc.materials)[x][y][z] = 1;
                    else if (value < 0.5) (*tc.materials)[x][y][z] = 2;
                    else (*tc.materials)[x][y][z] = 3;
                }
            }
        }
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}