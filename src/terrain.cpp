#include <iostream>
#include <list>

#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "anl.h"

#include "canton.h"
#include "terrain.h"
#include "shadercallback.h"

static int x_chunk = 32;
static int y_chunk = 32;
static int z_chunk = 32;

using namespace irr;

ScalarTerrain::ScalarTerrain()
{

    printf("Loading Terrain Document\n");

    noiseTree.loadFile("basicTerrain.xml");

    printf("Terrain Loaded\n");

    psFileName = "./shaders/terrain.frag";
    vsFileName = "./shaders/terrain.vert";

    if(IRR.gpu)
    {
        printf("Making a shader\n");
        ShaderCallBack * shaderCallBack = new ShaderCallBack();

        terrainMaterial = IRR.gpu->addHighLevelShaderMaterialFromFiles(
            vsFileName, "vertexMain", video::EVST_VS_1_1,
            psFileName, "pixelMain", video::EPST_PS_1_1,
            shaderCallBack, video::EMT_SOLID);

        shaderCallBack->drop();
    }

    Material.setFlag(irr::video::EMF_BACK_FACE_CULLING, true);
    Material.setFlag(irr::video::EMF_WIREFRAME, false);
    Material.setFlag(irr::video::EMF_LIGHTING, true);

    Material.setTexture(0, IRR.driver->getTexture("./resources/dirt.jpg"));
    Material.setTexture(1, IRR.driver->getTexture("./resources/clay.jpg"));
    Material.setTexture(2, IRR.driver->getTexture("./resources/grass.jpg"));
    Material.setTexture(3, IRR.driver->getTexture("./resources/rock.jpg"));
    Material.setTexture(4, IRR.driver->getTexture("./resources/sand.jpg"));
    Material.setTexture(5, IRR.driver->getTexture("./resources/void.jpg"));

    Material.MaterialType = (video::E_MATERIAL_TYPE) terrainMaterial;
}

void ScalarTerrain::GenerateBackground(TerrainLocation tl) 
{
    worldMap[tl].FillChunk(noiseTree);
}

void ScalarTerrain::MeshBackground(TerrainLocation tl) {
    worldMap[tl].MeshChunk();
    Mesh.addMeshBuffer(worldMap[tl].buf);
    Mesh.setDirty();
}

void TerrainChunk::MeshChunk()
{
    meshing = true;
    generateIsoSurface(* buf, * values, * materials, localPoint->X * x_chunk, localPoint->Y * y_chunk, localPoint->Z * z_chunk);
    meshing = false;
    clean = true;
}

void ScalarTerrain::generateMesh(irr::core::vector3df center) 
{
    for(int i = 0; i < Mesh.getMeshBufferCount(); i ++) {
        Mesh.MeshBuffers.erase(i);
    }

    int xCenter = ((int) center.X) / x_chunk;
    int yCenter = ((int) center.Y) / y_chunk;
    int zCenter = ((int) center.Z) / z_chunk;

    for(int z = zCenter-3; z <= zCenter+3; z++) {
        for(int y = yCenter-1; y <= yCenter+1; y++) {
            for(int x = xCenter-3; x <= xCenter+3; x++) {
                if(worldMap.find(TerrainLocation(x,y,z)) == worldMap.end())
                {
                    //printf("No Chunk - Adding (%i,%i,%i)\n",x,y,z);
                    worldMap[TerrainLocation(x,y,z)] = TerrainChunk(x_chunk, y_chunk, z_chunk, x, y, z);
                }
                if(!worldMap[TerrainLocation(x,y,z)].filled && !worldMap[TerrainLocation(x,y,z)].filling)
                {
                    //printf("Empty Chunk - filling (%i,%i,%i)\n",x,y,z);
                    std::thread t1(&ScalarTerrain::GenerateBackground, this, TerrainLocation(x,y,z));
                    t1.detach(); //Background Operations
                    //t1.join(); //Foreground Operations
                }
                else if(
                    !worldMap[TerrainLocation(x,y,z)].clean && 
                    !worldMap[TerrainLocation(x,y,z)].filling && 
                    !worldMap[TerrainLocation(x,y,z)].meshing &&
                    worldMap[TerrainLocation(x,y,z)].filled
                    )
                {
                    //printf("Unclean Chunk - rendering (%i,%i,%i)\n",x,y,z);
                    std::thread t1(&ScalarTerrain::MeshBackground, this, TerrainLocation(x,y,z));
                    //t1.detach();
                    t1.join(); //I can't do this in the background yet.
                    //worldMap[TerrainLocation(x,y,z)].MeshChunk();
                    
                    //Mesh.setDirty();
                }
                else if(worldMap[TerrainLocation(x,y,z)].clean)
                {
                    //printf("Clean Chunk - grabbing from cache (%i,%i,%i)\n",x,y,z);
                    Mesh.addMeshBuffer(worldMap[TerrainLocation(x,y,z)].buf);
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