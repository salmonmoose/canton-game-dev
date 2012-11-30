#include "terrain.h"


ScalarTerrain::ScalarTerrain()
{
}

bool PointInFrustum(const irr::scene::SViewFrustum * Frustum, const irr::core::vector3df & Point)
{
    for (int i=0; i<6; i++) {
        if(Frustum->planes[i].classifyPointRelation(Point) == irr::core::ISREL3D_FRONT) return false;
    }

    return true;
}

bool AABBoxInFrustum(const irr::scene::SViewFrustum * Frustum, const irr::core::aabbox3df & Box)
{
    //loop through each box point.
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MinEdge.Y, Box.MinEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MinEdge.Y, Box.MinEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MaxEdge.Y, Box.MinEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MinEdge.Y, Box.MaxEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MaxEdge.Y, Box.MinEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MinEdge.Y, Box.MaxEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MinEdge.X, Box.MaxEdge.Y, Box.MaxEdge.Z))) return true;
    if(PointInFrustum(Frustum, irr::core::vector3df(Box.MaxEdge.X, Box.MaxEdge.Y, Box.MaxEdge.Z))) return true;

    return false;
}

void ScalarTerrain::Init()
{
    printf("Loading Terrain Document\n");

    noiseTree.loadFile("basicTerrain.xml");

    printf("Terrain Loaded\n");

    vsFileName = "./shaders/terrain.vert";
    psFileName = "./shaders/terrain.frag";

    if(IRR.gpu)
    {
        printf("Making a shader\n");
        ShaderCallback * shaderCallback = new ShaderCallback();

        terrainMaterial = IRR.gpu->addHighLevelShaderMaterialFromFiles(
            vsFileName, "vertexMain", video::EVST_VS_1_1,
            psFileName, "pixelMain", video::EPST_PS_1_1,
            shaderCallback, video::EMT_SOLID);

        shaderCallback->drop();
    }

    Material.setFlag(irr::video::EMF_BACK_FACE_CULLING, true);
    Material.setFlag(irr::video::EMF_WIREFRAME, false);
    Material.setFlag(irr::video::EMF_LIGHTING, true);

    //Material.diffuseColor(irr::video::SColor(255,255,196,255));
    Material.setTexture(0, IRR.driver->getTexture("./resources/grass.jpg"));
    Material.setTexture(1, IRR.driver->getTexture("./resources/dirt.jpg"));
    Material.setTexture(2, IRR.driver->getTexture("./resources/dirt.jpg"));
    Material.setTexture(3, IRR.driver->getTexture("./resources/clay.jpg"));
    Material.setTexture(4, IRR.driver->getTexture("./resources/sand.jpg"));
    Material.setTexture(5, IRR.driver->getTexture("./resources/void.jpg"));

    Material.MaterialType = (video::E_MATERIAL_TYPE) terrainMaterial;  
}

void ScalarTerrain::FillBackground(irr::core::vector3d<int> tl) 
{
    worldMap[tl].FillChunk(noiseTree);
    threads--;
    fillThreads--;
    worldMap[tl].status = DIRTY;
}

void ScalarTerrain::MeshBackground(irr::core::vector3d<int> tl) {
    worldMap[tl].MeshChunk();
    threads--;
    meshThreads--;
    worldMap[tl].status = CLEAN;
}

int ScalarTerrain::GetAltitude(const irr::core::vector3df & Position)
{
    return GetAltitude(irr::core::vector3d<int>(
        (int) Position.X,
        (int) Position.Y,
        (int) Position.Z
    ));
}

int ScalarTerrain::GetAltitude(const irr::core::vector3d<int> & Position)
{
    unsigned y;

    int height;

    for(y = 0; y < 16; y++) {
        VoxelReference vr(irr::core::vector3d<int>(Position.X, Position.Y - (y * dimensions.Y), Position.Z), dimensions);

        worldMap_iterator = worldMap.find(vr.Chunk);

        if(worldMap_iterator != worldMap.end())
        {
            height = worldMap_iterator->second.GetHeight(irr::core::vector2d<unsigned>(vr.Position.X, vr.Position.Z));

            if(height >= 0)
            {
                return (vr.Chunk.Y * dimensions.Y) + height;
            }
        }
    }

    return Position.Y - (y * dimensions.Y);
}

bool ScalarTerrain::GetFilled(const irr::core::vector3df & Position)
{
    return GetFilled(irr::core::vector3d<int>(
        (int) Position.X,
        (int) Position.Y,
        (int) Position.Z
    ));
}

bool ScalarTerrain::GetFilled(const irr::core::vector3d<int> & Position)
{
    VoxelReference vr(Position, dimensions);

    worldMap_iterator = worldMap.find(vr.Chunk);

    if(worldMap_iterator != worldMap.end())
    {
        return worldMap_iterator->second.GetFilled(vr.Position);
    }
    else
    {
        return false;
    }
}


void ScalarTerrain::UpdateVoxel(VoxelData & vd)
{
    VoxelReference vr(vd.Position, dimensions);

    worldMap_iterator = worldMap.find(vr.Chunk);

    if(worldMap_iterator != worldMap.end())
    {
        worldMap_iterator->second.UpdateVoxel(vr.Position, vd.Value, vd.Material);
    }
}

void ScalarTerrain::generateMesh(const irr::scene::SViewFrustum * Frustum) 
{
    irr::core::aabbox3df bounding = Frustum->getBoundingBox();

    int x_start = (int)floor(bounding.MinEdge.X / dimensions.X);
    int y_start = (int)floor(bounding.MinEdge.Y / dimensions.Y);
    int z_start = (int)floor(bounding.MinEdge.Z / dimensions.Z);

    int x_finish = (int)ceil(bounding.MaxEdge.X / dimensions.X);
    int y_finish = (int)ceil(bounding.MaxEdge.Y / dimensions.Y);
    int z_finish = (int)ceil(bounding.MaxEdge.Z / dimensions.Z);

    //HACK: This is a naive hack to prevent bottoming out the CPU, elegance required.
    if((x_finish - x_start) * (y_finish - y_start) * (z_finish - z_start) > 10000)
    {
        printf("too many buffers\n");
        return;
    }           

    for(unsigned i = 0; i < Mesh.getMeshBufferCount(); i ++) {
        Mesh.MeshBuffers.erase(i);
    }

    int boxCount = 0;
    int frustumCount = 0;
    int actualCount = 0;
    irr::core::vector3d<int> tl(0,0,0);

    irr::core::aabbox3df aabbox;

    for(int y = y_finish; y >= y_start; y--) { //start picking from the top.
        for(int z = z_finish; z >= z_start; z--) { //todo should probably pick towards player first
            for(int x = x_start; x <= x_finish; x++) {
                boxCount ++;

                tl.set(x,y,z);
                
                if(worldMap.find(tl) == worldMap.end())
                {
                    worldMap[tl] = TerrainChunk();
                    worldMap[tl].Initialize(dimensions, irr::core::vector3d<int>(x,y,z));
                }

                aabbox = irr::core::aabbox3df(
                    (dimensions.X * x), 
                    (dimensions.Y * y), 
                    (dimensions.Z * z),
                    (dimensions.X * x + dimensions.X), 
                    (dimensions.Y * y + dimensions.Y), 
                    (dimensions.Z * z + dimensions.Z)
                );

                if(AABBoxInFrustum(Frustum, aabbox))
                {
                    frustumCount ++;
                    
                    if(!worldMap[irr::core::vector3d<int>(tl.X, tl.Y+1, tl.Z)].obstruct) 
                    { //If the layer above me doesn't obsrtuct, render
                        
                        /**
                        * if the chunk is Clean, just pass the buffer empty chunk will never be made clean
                        */
                        if(worldMap[tl].status == CLEAN)
                        {
                            actualCount++;
                            Mesh.addMeshBuffer(worldMap[tl].buffer);
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
                            worldMap[tl].status = FILLING;
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

    //FIXME: This should be a function of the engine, query the nodes externally you naughty boy.
    IRR.boxBuffers->setText(core::stringw(L"AABBox Buffers: ").append(core::stringw(boxCount)).c_str());
    IRR.frustumBuffers->setText(core::stringw(L"Frustum Buffers: ").append(core::stringw(frustumCount)).c_str());
    IRR.actualBuffers->setText(core::stringw(L"Actual Buffers: ").append(core::stringw(actualCount)).c_str());
    IRR.meshThreads->setText(core::stringw(L"Mesh Threads: ").append(core::stringw(meshThreads)).c_str());
    IRR.fillThreads->setText(core::stringw(L"Fill Threads: ").append(core::stringw(fillThreads)).c_str());
    //Mesh.recalculateBoundingBox();
    //printf("Mesh has %i buffers\n", Mesh.getMeshBufferCount());
}