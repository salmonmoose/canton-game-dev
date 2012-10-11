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

    Material.setTexture(0, IRR.driver->getTexture("./resources/grass.jpg"));
    Material.setTexture(1, IRR.driver->getTexture("./resources/rock.jpg"));
    Material.setTexture(2, IRR.driver->getTexture("./resources/dirt.jpg"));
    Material.setTexture(3, IRR.driver->getTexture("./resources/clay.jpg"));
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
    generateIsoSurface(* buf, * values, * materials, localPoint->X * x_chunk, localPoint->Y * y_chunk, localPoint->Z * z_chunk);
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

    if((x_finish - x_start) * (y_finish - y_start) * (z_finish - z_start) > 10000)
    {
        printf("too many buffers");
        return;
    }           

    for(int i = 0; i < Mesh.getMeshBufferCount(); i ++) {
        Mesh.MeshBuffers.erase(i);
    }

    int boxCount = 0;
    int frustumCount = 0;
    int actualCount = 0;
    TerrainLocation tl(0,0,0);

    for(int y = y_finish; y >= y_start; y--) { //start picking from the top.
        for(int z = z_finish; z >= z_start; z--) { //todo should probably pick towards player first
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
                    
                    if(!worldMap[TerrainLocation(tl.X, tl.Y+1, tl.Z)].obstruct) 
                    { //If the layer above me doesn't obsrtuct, render
                        
                        /**
                        * if the chunk is Clean, just pass the buffer empty chunk will never be made clean
                        */
                        if(worldMap[tl].status == CLEAN)
                        {
                            actualCount++;
                            Mesh.addMeshBuffer(worldMap[tl].buf);
                        }

                        /**
                        * if the chunk is filled, unmeshed, there are threads free and there is data, render
                        */
                        if((worldMap[tl].status == FILLED || worldMap[tl].status == DIRTY) && threads < MAXTHREADS && !worldMap[tl].empty)
                        {
                            worldMap[tl].status = MESHING;
                            threads++;
                            meshThreads++;
                            std::thread t1(&ScalarTerrain::MeshBackground, this, tl);
                            //t1.detach();
                            t1.join(); //This still doesn't quite work
                        }

                        /**
                        * if the chunk has no data, and there are threads free, generate chunk data.
                        */
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
                    else
                    {
                        worldMap[tl].obstruct = true;
                    }
                }
            }
        }
    }

    IRR.boxBuffers->setText(core::stringw(L"AABBox Buffers: ").append(core::stringw(boxCount)).c_str());
    IRR.frustumBuffers->setText(core::stringw(L"Frustum Buffers: ").append(core::stringw(frustumCount)).c_str());
    IRR.actualBuffers->setText(core::stringw(L"Actual Buffers: ").append(core::stringw(actualCount)).c_str());
    IRR.meshThreads->setText(core::stringw(L"Mesh Threads: ").append(core::stringw(meshThreads)).c_str());
    IRR.fillThreads->setText(core::stringw(L"Fill Threads: ").append(core::stringw(fillThreads)).c_str());
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

    bool solid;

    try {
        for(int y = 0; y <= y_chunk; y++) {
            solid=true; //fresh layer, assume solid
            for(int x = 0; x <= x_chunk; x++) {
                for(int z = 0; z <= z_chunk; z++) {
                    
                    value = noiseTree.get(
                        (double) (x + xPos) / 32.f, // (double) x_chunk, 
                        (double) (y + yPos) / 32.f, // (double) y_chunk, 
                        (double) (z + zPos) / 32.f // (double) z_chunk
                    );

                    if(value > 0.5) {
                        empty = false; //a value has been found, block must be meshed
                    } else {
                        solid = false; //a hole has been found, this layer is not solid
                    }

                    (*values)[x][y][z] = value;
                    (*materials)[x][y][z] = 0;
                }
            }
            if(solid) obstruct=true; //if an entire layer is solid, chunk obstructs vertically
        }
        //Chunk is clean, allow rendering.
    } catch (char * exception) {
        printf("Exception raised: %s\n", exception);
    }
}