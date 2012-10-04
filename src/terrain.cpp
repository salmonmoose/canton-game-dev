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

    printf("Terrain Loaded\n");
}

void ScalarTerrain::GenerateBackground(TerrainLocation tl) 
{
    printf(
        "Starting Threadded Generation at (%i,%i,%i)\n",
        tl.X, tl.Y, tl.Z
        );
    
    worldMap[tl].FillChunk(noiseTree);
    
    printf(
        "Finished Threadded Generation at (%i,%i,%i)\n",
        tl.X, tl.Y, tl.Z
        );
}

void ScalarTerrain::MeshBackground(TerrainLocation tl) {
    printf(
        "Starting Threadded Meshing at (%i,%i,%i)\n",
        tl.X, tl.Y, tl.Z
        );

    worldMap[tl].MeshChunk();
    Mesh.addMeshBuffer(worldMap[tl].buf);
    Mesh.setDirty();

    printf(
        "Finished Threadded Meshing at (%i,%i,%i)\n",
        tl.X, tl.Y, tl.Z
        );
}

void TerrainChunk::MeshChunk()
{
    printf("Starting Mesh Generation\n");
    generateIsoSurface(* buf, * values, * materials, localPoint->X * x_chunk, localPoint->Y * y_chunk, localPoint->Z * z_chunk);
    clean = true;
    printf("Mesh Generated\n");
}

void ScalarTerrain::generateMesh(irr::core::vector3df center) 
{
    int xCenter = ((int) center.X) / x_chunk;
    int yCenter = ((int) center.Y) / y_chunk;
    int zCenter = ((int) center.Z) / z_chunk;

    for(int z = zCenter-1; z <= zCenter+1; z++) {
        for(int y = yCenter-1; y <= yCenter+1; y++) {
            for(int x = xCenter-1; x <= xCenter+1; x++) {
                if(worldMap.find(TerrainLocation(x,y,z)) == worldMap.end())
                {
                    printf("No Chunk - Adding (%i,%i,%i)\n",x,y,z);
                    worldMap[TerrainLocation(x,y,z)] = TerrainChunk(x_chunk, y_chunk, z_chunk, x, y, z);
                }
                if(!worldMap[TerrainLocation(x,y,z)].filled && !worldMap[TerrainLocation(x,y,z)].filling)
                {
                    printf("Empty Chunk - filling (%i,%i,%i)\n",x,y,z);
                    std::thread t1(&ScalarTerrain::GenerateBackground, this, TerrainLocation(x,y,z));
                    t1.detach(); //Background Operations
                    //t1.join(); //Foreground Operations
                }
                else if(
                    !worldMap[TerrainLocation(x,y,z)].clean && 
                    !worldMap[TerrainLocation(x,y,z)].filling && 
                    worldMap[TerrainLocation(x,y,z)].filled
                    )
                {
                    printf("Unclean Chunk - rendering (%i,%i,%i)\n",x,y,z);
                    std::thread t1(&ScalarTerrain::MeshBackground, this, TerrainLocation(x,y,z));
                    //t1.detach();
                    t1.join();
                    //worldMap[TerrainLocation(x,y,z)].MeshChunk();
                    
                    //Mesh.setDirty();
                }
                else if(worldMap[TerrainLocation(x,y,z)].clean)
                {
                    //Mesh.addMeshBuffer(worldMap[TerrainLocation(x,y,z)].buf);
                }
            }
        }
    }
    //Mesh.recalculateBoundingBox();
    //printf("Mesh has %i buffers\n", Mesh.getMeshBufferCount());
}

/**
Using noise tree, populates an array with values
**/
void TerrainChunk::FillChunk(anl::CImplicitXML & noiseTree) {
    filling = true;
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
        filled = true;
        filling = false;
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}