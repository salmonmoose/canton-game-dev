#include "terrain.h"

static irr::core::vector3d<int> Previous[6] = {
    irr::core::vector3d<int>(0,0,1),
    irr::core::vector3d<int>(0,1,0),
    irr::core::vector3d<int>(0,1,1),
    irr::core::vector3d<int>(1,0,0),
    irr::core::vector3d<int>(1,0,1),
    irr::core::vector3d<int>(1,1,1)
};


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

    for(int i = 0; i < 6; i++)
    {
        irr::core::vector3d<int> clean(tl.X - Previous[i].X, tl.Y - Previous[i].Y, tl.Z - Previous[i].Z);
        
        if(worldMap[clean].status == CLEAN || worldMap[clean].status == MESHING)
        {
            worldMap[clean].status = DIRTY;
        }

        //FIXME: there is an edge case where this chunk may being rendered
        //Perhaps after being meshed, chunks should only change to "CLEAN" if their status is "MESHING"
    }

    threads--;
    fillThreads--;
    worldMap[tl].status = FILLED;
}

void ScalarTerrain::MeshBackground(irr::core::vector3d<int> tl) 
{
    worldMesh[tl].Initialize(this, tl);
    worldMesh[tl].GenerateMesh();
    
    if(worldMap[tl].status == MESHING) 
        worldMap[tl].status = CLEAN;

    threads--;
    meshThreads--;
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
        VoxelReference vr(irr::core::vector3d<int>(Position.X, Position.Y - (y * dimensions.Y), Position.Z));

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
    VoxelReference vr(Position);

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

void ScalarTerrain::UpdateVoxel(VoxelData & vd, bool subtract)
{
    VoxelReference vr(vd.Position);

    worldMap_iterator = worldMap.find(vr.Chunk);

    if(worldMap_iterator != worldMap.end())
    {
        worldMap_iterator->second.UpdateVoxel(vr.Position, vd.Value, vd.Material, subtract);
        worldMap_iterator->second.status = DIRTY;

        if(vr.Position.X == 0)
            worldMap[irr::core::vector3d<int>(vr.Chunk.X - 1, vr.Chunk.Y, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.X == dimensions.X -1)
            worldMap[irr::core::vector3d<int>(vr.Chunk.X + 1, vr.Chunk.Y, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.Y == 0)
            worldMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y - 1, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.Y == dimensions.Y -1)
            worldMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y + 1, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.Z == 0)
            worldMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y, vr.Chunk.Z - 1)].status = DIRTY;
        if(vr.Position.Z == dimensions.Z -1)
            worldMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y, vr.Chunk.Z + 1)].status = DIRTY;
    }
}

void ScalarTerrain::AddBrush(irr::core::vector3df Position)
{
    float radius = 6.f;

    double len;
    double value;
    double dx,dy,dz;


    for(int x = (int)Position.X - (int) radius; x <= (int)Position.X + (int)radius; x++)
    {
        for(int y = (int)Position.Y - (int) radius; y <= (int)Position.Y + (int)radius; y++)
        {
            for(int z = (int)Position.Z - (int) radius; z <= (int)Position.Z + (int)radius; z++)
            {
                dx = x - Position.X;
                dy = y - Position.Y;
                dz = z - Position.Z;

                len = sqrt(
                    (double) dx * (double) dx + 
                    (double) dy * (double) dy + 
                    (double) dz * (double) dz
                    );
                value = (radius - len)/radius;
                if (value < 0) value = 0;
                if (value > 1) value = 1;

                VoxelData vd(irr::core::vector3d<int>(x,y,z), value, 1);

                //printf("Changing point (%i,%i,%i), %f\n", x,y,z, value);
                UpdateVoxel(vd, false);
            }
        }
    }
}

void ScalarTerrain::RemoveBrush(irr::core::vector3df Position)
{
   float radius = 6.f;

    double len;
    double value;
    double dx,dy,dz;


    for(int x = (int)Position.X - (int) radius; x <= (int)Position.X + (int)radius; x++)
    {
        for(int y = (int)Position.Y - (int) radius; y <= (int)Position.Y + (int)radius; y++)
        {
            for(int z = (int)Position.Z - (int) radius; z <= (int)Position.Z + (int)radius; z++)
            {
                dx = x - Position.X;
                dy = y - Position.Y;
                dz = z - Position.Z;

                len = sqrt(
                    (double) dx * (double) dx + 
                    (double) dy * (double) dy + 
                    (double) dz * (double) dz
                    );
                value = (radius - len)/radius;
                if (value < 0) value = 0;
                if (value > 1) value = 1;

                VoxelData vd(irr::core::vector3d<int>(x,y,z), value, 1);

                //printf("Changing point (%i,%i,%i), %f\n", x,y,z, value);
                UpdateVoxel(vd, true);
            }
        }
    }
}

float ScalarTerrain::GetValue(irr::core::vector3d<int> Position)
{
    VoxelReference vr(Position);

    worldMap_iterator = worldMap.find(vr.Chunk);

    //printf("(%i,%i,%i)=(%i,%i,%i)+(%i,%i,%i)\n", Position.X, Position.Y, Position.Z, vr.Position.X, vr.Position.Y, vr.Position.Z, vr.Chunk.X, vr.Chunk.Y, vr.Chunk.Z);

    if(worldMap_iterator != worldMap.end())
    {
        //return 1.f;
        return worldMap_iterator->second.GetValue(vr.Position);
    }
    else
    {
        return -10.f;
    }
}

int ScalarTerrain::GetMaterial(irr::core::vector3d<int> Position)
{
    VoxelReference vr(Position);

    worldMap_iterator = worldMap.find(vr.Chunk);

    if(worldMap_iterator != worldMap.end())
    {
        return worldMap_iterator->second.GetMaterial(vr.Position);
    }
    else
    {
        return 0;
    }
}


//                                         .o8                        oooo                                 
//                                        "888                        `888                                 
//    oooo d8b  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b     888   .ooooo.   .ooooo.  oo.ooooo.  
//    `888""8P d88' `88b `888P"Y88b  d88' `888  d88' `88b `888""8P     888  d88' `88b d88' `88b  888' `88b 
//     888     888ooo888  888   888  888   888  888ooo888  888         888  888   888 888   888  888   888 
//     888     888    .o  888   888  888   888  888    .o  888         888  888   888 888   888  888   888 
//    d888b    `Y8bod8P' o888o o888o `Y8bod88P" `Y8bod8P' d888b       o888o `Y8bod8P' `Y8bod8P'  888bod8P' 
//                                                                                               888       
//                                                                                              o888o      
                                                                                                         

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
/*
    x_start = -2;
    x_finish = -2;
    y_start = -2;
    y_finish = -2;
    z_start = -2;
    z_finish = -2;
*/

    for(int y = y_finish; y >= y_start; y--) { //start picking from the top.
        for(int z = z_finish; z >= z_start; z--) { //todo should probably pick towards player first
            for(int x = x_start; x <= x_finish; x++) {
                boxCount ++;
                tl.set(x,y,z);
                
                if(worldMap.find(tl) == worldMap.end())
                {
                    worldMap[tl] = TerrainChunk();
                    //printf("Adding: (%i,%i,%i)\n",tl.X, tl.Y, tl.Z);
                    worldMap[tl].Initialize(dimensions, irr::core::vector3d<int>(x,y,z));
                }

                aabbox = irr::core::aabbox3df(
                    ((int)dimensions.X * x), 
                    ((int)dimensions.Y * y), 
                    ((int)dimensions.Z * z),
                    ((int)dimensions.X * x + (int)dimensions.X), 
                    ((int)dimensions.Y * y + (int)dimensions.Y), 
                    ((int)dimensions.Z * z + (int)dimensions.Z)
                );

                if(AABBoxInFrustum(Frustum, aabbox))
                {
                    frustumCount ++;
                    
                    if(!worldMap[irr::core::vector3d<int>(tl.X, tl.Y+1, tl.Z)].obstruct) 
                    { //If the layer above me doesn't obsrtuct, render
                        
                        /**
                        * if the chunk is Clean, or Dirty a mesh exists - dirty meshes are out of date but the best we have.
                        */
                        if(worldMesh[tl].Meshed)
                        {
                            actualCount++;
                            Mesh.addMeshBuffer(worldMesh[tl].buffer);
                            //Mesh.addMeshBuffer(worldMap[tl].buffer);
                        }

                        /**
                        * if the chunk is filled, unmeshed, there are threads free and there is data, render
                        */

                        //FIXME - if this chunk is already meshing, and is dirty again, the process should be thrown out and restarted.
                        if((worldMap[tl].status == FILLED || worldMap[tl].status == DIRTY) && threads < MAXTHREADS && !worldMap[tl].empty)
                        {
                            //printf("Meshing: (%i,%i,%i)\n", tl.X, tl.Y, tl.Z);
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
                            //printf("Filling: (%i,%i,%i)\n", tl.X, tl.Y, tl.Z);
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
                        y = y_start; //We don't need to check anything else in this column.
                    }
                }
            }
        }
    }

    //Mesh.recalculateBoundingBox();
    //printf("Mesh has %i buffers\n", Mesh.getMeshBufferCount());
}