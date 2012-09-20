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
void ScalarTerrain::getMesh(/*frustum*/)
{
    //Loop through all chunks in frustum
    printf("Spawn Chunk\n");

    for(int z = -1; z <= 1; z++) {
        for(int x = -1; x <= 1; x++) {
            printf("generating chunk %i,0,%i ... ",x,z);
            worldMap[TerrainLocation(x,0,z)] = TerrainChunk(x_chunk,y_chunk,z_chunk,x,0,z);
            renderChunk(worldMap[TerrainLocation(x,0,z)]);
        }
    }
}

void ScalarTerrain::generateMesh() {
    int y = 0;
    for(int z = -1; z <= 1; z++) {
        for(int x = -1; x <= 1; x++) {
            printf("setting up buf %i,%i,%i ... ",x,y,z);
            generateIsoSurface(
                Mesh,
                (* worldMap[TerrainLocation(x,0,z)].values),
                (* worldMap[TerrainLocation(x,0,z)].materials),
                x * x_chunk, y * y_chunk, z * z_chunk
            );
            printf("buf setup\n");
        }
    }
}

void ScalarTerrain::generateNavMesh()
{

}

/**
Using noise tree, populates an array with values
**/
void ScalarTerrain::renderChunk(TerrainChunk &tc) {
    double xPos = tc.localPoint->X * x_chunk;
    double yPos = tc.localPoint->Y * y_chunk;
    double zPos = tc.localPoint->Z * z_chunk;

    try {
        for(int z = 0; z < z_chunk; z++) {
            for(int y = 0; y < y_chunk; y++) {
                for(int x = 0; x < x_chunk; x++) {
                    
                    value = noiseTree.get(
                        ((double)x/x_chunk) + xPos, 
                        ((double)y/y_chunk) + yPos, 
                        ((double)z/z_chunk) + zPos
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