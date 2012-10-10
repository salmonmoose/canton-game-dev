#include <iostream>
#include <list>

#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include "anl.h"

#include "canton.h"
#include "terrain.h"
#include "shadercallback.h"

static int x_chunk = 16;
static int y_chunk = 16;
static int z_chunk = 16;

using namespace irr;

bool pointInFrustum(const irr::scene::SViewFrustum * Frustum, const irr::core::vector3df & Point)
{
    for (int i=0; i<6; i++) {
        if(Frustum->planes[i].classifyPointRelation(Point) == irr::core::ISREL3D_FRONT) return false;
    }

    return true;
}

bool AABBoxInFrustum(const irr::scene::SViewFrustum * Frustum, const irr::core::aabbox3df & Box)
{
    //loop through each box point.
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MinEdge.Y, Box.MinEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MinEdge.Y, Box.MinEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MaxEdge.Y, Box.MinEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MinEdge.Y, Box.MaxEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MaxEdge.Y, Box.MinEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MinEdge.Y, Box.MaxEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MaxEdge.Y, Box.MaxEdge.Z))) return true;
    if(pointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MaxEdge.Y, Box.MaxEdge.Z))) return true;

    return false;
}

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

void ScalarTerrain::FillBackground(TerrainLocation tl) 
{
    worldMap[tl].FillChunk(noiseTree);
    threads--;
    fillThreads--;
    worldMap[tl].status = DIRTY;
}

void ScalarTerrain::MeshBackground(TerrainLocation tl) {
    worldMap[tl].MeshChunk();
    threads--;
    meshThreads--;
    worldMap[tl].status = CLEAN;
}

void TerrainChunk::MeshChunk()
{
    meshing = true;
    generateIsoSurface(* buf, * values, * materials, localPoint->X * x_chunk, localPoint->Y * y_chunk, localPoint->Z * z_chunk);
    meshing = false;
    clean = true;
}

void ScalarTerrain::generateMesh(const irr::scene::SViewFrustum * Frustum) 
{
    irr::core::aabbox3df bounding = Frustum->getBoundingBox();

    int x_start = (int)floor(bounding.MinEdge.X / x_chunk);
    int y_start = (int)floor(bounding.MinEdge.Y / y_chunk);
    int z_start = (int)floor(bounding.MinEdge.Z / z_chunk);

    int x_finish = (int)ceil(bounding.MaxEdge.X / x_chunk);
    int y_finish = (int)ceil(bounding.MaxEdge.Y / y_chunk);
    int z_finish = (int)ceil(bounding.MaxEdge.Z / z_chunk);

    int mesh_request = abs(x_finish - x_start) * abs(y_finish - y_start) * abs(z_finish - z_start);

    if(mesh_request > 5000) 
    {
        printf("too many chunks\n");
    return; //Don't try to work with too many chunks this should be more elegant.
    }

    for(int i = 0; i < Mesh.getMeshBufferCount(); i ++) {
        Mesh.MeshBuffers.erase(i);
    }

    int boxCount = 0;
    int frustumCount = 0;

    TerrainLocation tl(0,0,0);

    for(int z = z_start; z <= z_finish; z++) {
        for(int y = y_start; y <= y_finish; y++) {
            for(int x = x_start; x <= x_finish; x++) {
                boxCount ++;

                tl.set(x,y,z);
                
                if(worldMap.find(tl) == worldMap.end())
                {
                    worldMap[tl] = TerrainChunk(x_chunk, y_chunk, z_chunk, x, y, z);
                }
                if(AABBoxInFrustum(Frustum, worldMap[tl].buf->getBoundingBox()))
                {
                    frustumCount ++;
                    if(worldMap[tl].status == CLEAN)
                    {
                        Mesh.addMeshBuffer(worldMap[tl].buf);
                    }

                    if((worldMap[tl].status == FILLED || worldMap[tl].status == DIRTY) && threads < MAXTHREADS)
                    {
                        worldMap[tl].status = MESHING;
                        threads++;
                        meshThreads++;
                        std::thread t1(&ScalarTerrain::MeshBackground, this, tl);
                        //t1.detach();
                        t1.join(); //This still doesn't quite work
                    }

                    if(worldMap[tl].status == EMPTY && threads < MAXTHREADS)
                    {
                        worldMap[tl].status == FILLING;
                        threads++;
                        fillThreads++;
                        std::thread t1(&ScalarTerrain::FillBackground, this, tl);
                        t1.detach(); //Background Operations
                        //t1.join(); //Foreground Operations
                    }
                }
            }
        }
    }

    IRR::boxBuffers.setText();
    IRR::frustumBuffers.setText();
    IRR::meshThreads.setText();
    IRR::fillThreads.setText();
    //Mesh.recalculateBoundingBox();
    //printf("Mesh has %i buffers\n", Mesh.getMeshBufferCount());
}

/**
Using noise tree, populates an array with values
**/
void TerrainChunk::FillChunk(anl::CImplicitXML & noiseTree) {
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
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}