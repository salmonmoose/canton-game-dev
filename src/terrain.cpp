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

ScalarTerrain::ScalarTerrain() : BackgroundTask(1)
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



    for(int z = -4; z <= 4; z++) {
        for(int x = -4; x <= 4; x++) {
            printf("generating chunk %i,0,%i ... ",x,z);
            worldMap[TerrainLocation(x,0,z)] = TerrainChunk(x_chunk,y_chunk,z_chunk,x,0,z);
            GenerateBackground(TerrainLocation(x,0,z));
        }
    }
}

void ScalarTerrain::GenerateBackground(TerrainLocation tl) 
{
    addBackgroundTask(boost::bind(&worldMap[tl].renderChunk, this, noiseTree));
}

void ScalarTerrain::generateMesh() 
{
    int y = 0;
    for(int z = -4; z <= 4; z++) {
        for(int x = -4; x <= 4; x++) {
            printf("setting up buf %i,%i,%i ... ",x,y,z);
            generateIsoSurface(
                Mesh,
                (* worldMap[TerrainLocation(x,y,z)].values),
                (* worldMap[TerrainLocation(x,y,z)].materials),
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
void TerrainChunk::renderChunk(anl::CImplicitXML & noiseTree) {
    double value;
    double xPos = localPoint->X * x_chunk;
    double yPos = localPoint->Y * y_chunk;
    double zPos = localPoint->Z * z_chunk;

    try {
        for(int z = 0; z <= z_chunk; z++) {
            for(int y = 0; y <= y_chunk; y++) {
                for(int x = 0; x <= x_chunk; x++) {
                    
                    value = noiseTree.get(
                        (x + xPos) / (double) x_chunk, 
                        (y + yPos) / (double) y_chunk, 
                        (z + zPos) / (double) z_chunk
                    );

                    (*values)[x][y][z] = value;

                    if(value < -0.5) (*materials)[x][y][z] = 0;
                    else if (value < 0) (*materials)[x][y][z] = 1;
                    else if (value < 0.5) (*materials)[x][y][z] = 2;
                    else (*materials)[x][y][z] = 3;
                }
            }
        }
        //Chunk is clean, allow rendering.
        clean = true;
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}