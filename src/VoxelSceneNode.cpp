#include "VoxelSceneNode.h"
#include <thread>

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


VoxelSceneNode::VoxelSceneNode(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * manager, s32 id) : irr::scene::IMeshSceneNode(parent, manager, id)
{
	Material.Wireframe = false;
	Material.Lighting = false;
	threads = 0;

	Mesh = new irr::scene::SMesh();

	setAutomaticCulling(scene::EAC_OFF);
}

bool VoxelSceneNode::Initialize()
{
	printf("Loading Terrain Document\n");

	noiseTree.loadFile("basicTerrain.xml");

	printf("Terrain Loaded\n");

	if(IRR.gpu)
	{
		vsFileName = "./shaders/terrain.vert.glsl";
		psFileName = "./shaders/terrain.frag.glsl";
		
	    ShaderCallback * shaderCallback = new ShaderCallback();

	    terrainMaterial = IRR.gpu->addHighLevelShaderMaterialFromFiles(
	        vsFileName, "vertexMain", video::EVST_VS_3_0,
	        psFileName, "pixelMain", video::EPST_PS_3_0,
	        shaderCallback, video::EMT_SOLID);

	    shaderCallback->drop();
	}

	Material.setFlag(irr::video::EMF_BACK_FACE_CULLING, false);
	Material.setFlag(irr::video::EMF_WIREFRAME, false);
	Material.setFlag(irr::video::EMF_LIGHTING, false);

	Material.MaterialType = (video::E_MATERIAL_TYPE) terrainMaterial;  

    setMaterialTexture(1, IRR.driver->getTexture("./resources/terrain.png"));
    setMaterialTexture(2, IRR.driver->getTexture("./resources/dirt.jpg"));
    setMaterialTexture(3, IRR.driver->getTexture("./resources/dirt.jpg"));
    setMaterialTexture(4, IRR.driver->getTexture("./resources/clay.jpg"));
    setMaterialTexture(5, IRR.driver->getTexture("./resources/sand.jpg"));
    setMaterialTexture(6, IRR.driver->getTexture("./resources/void.jpg"));

	dirty = true;

	return true;
}

void VoxelSceneNode::OnRegisterSceneNode()
{
	if(!IsVisible || !SceneManager->getActiveCamera())
		return;

	SceneManager->registerNodeForRendering(this);

	preRenderCalculationsIfNeeded();

	//ISceneNode::OnRegisterSceneNode();
}

void VoxelSceneNode::preRenderFillChunks()
{
	irr::core::vector3d<int> playerChunk = VoxelReference(
        irr::core::vector3d<int>(
            (int)IRR.mPlayer->getPosition().X,
            (int)IRR.mPlayer->getPosition().Y,
            (int)IRR.mPlayer->getPosition().Z
        )
    ).Chunk;

    irr::core::vector3d<int> localSpace(5,0,5);

    irr::core::vector3d<int> chunkPos(0,0,0);

    //Loop through local space - generate blocks downwards from ship
    for(int x = playerChunk.X - localSpace.X; x <= playerChunk.X + localSpace.X; x++)
    {
		for(int z = playerChunk.Z - localSpace.Z; z <= playerChunk.Z + localSpace.Z; z++)
		{
            chunkPos.set(x,playerChunk.Y,z);

            if(chunkMap.find(chunkPos) == chunkMap.end())
            {
                //printf("Initializing (%i,%i,%i) to queue\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
                chunkMap[chunkPos] = VoxelChunk();
                chunkMap[chunkPos].Initialize(this, chunkPos);
            }

            if(chunkMap[chunkPos].status == EMPTY)
            {
                //printf("Adding (%i,%i,%i) to queue\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
                chunkMap[chunkPos].status = FILLING;
                boost::threadpool::schedule(
                    (*IRR.mThreadPool), 
                    boost::threadpool::prio_task_func(1, boost::bind(&VoxelSceneNode::FillBackground, this, chunkPos))
                );
            }
		}
	}
}

void VoxelSceneNode::preRenderMeshChunks()
{
    irr::core::vector3d<int> overDraw(1,0,1);

    irr::core::aabbox3df bounding = IRR.camera->getViewFrustum()->getBoundingBox();

    irr::core::vector3d<int> newAabboxStart(
        (int)floor(bounding.MinEdge.X / dimensions.X) - overDraw.X,
        (int)floor(bounding.MinEdge.Y / dimensions.Y) - overDraw.Y,
        (int)floor(bounding.MinEdge.Z / dimensions.Z) - overDraw.Z
    );

    irr::core::vector3d<int> newAabboxEnd(
        (int)ceil(bounding.MaxEdge.X / dimensions.X) + overDraw.X,
        (int)ceil(bounding.MaxEdge.Y / dimensions.Y) + overDraw.Y,
        (int)ceil(bounding.MaxEdge.Z / dimensions.Z) + overDraw.Z
    );

    if((newAabboxStart == aabboxStart && newAabboxEnd == aabboxEnd) && !dirty)
    {
        return;
    }

    aabboxStart = newAabboxStart;
    aabboxEnd = newAabboxEnd;

    Mesh->MeshBuffers.clear();

    irr::core::aabbox3df aabbox;

    irr::core::vector3d<int> chunkPos(0,0,0);

    dirty = false;

    for(int z = aabboxEnd.Z; z >= aabboxStart.Z; z--)
    {
        for(int x = aabboxEnd.X; x >= aabboxStart.X; x--)
        {
            for(int y = aabboxEnd.Y; y >= aabboxStart.Y; y--)
            {
                chunkPos.set(x,y,z);

                if(meshMap.find(chunkPos) == meshMap.end())
                {
                    meshMap[chunkPos] = ChunkMesh();
                    meshMap[chunkPos].Initialize(this, chunkPos);
                }

                aabbox = irr::core::aabbox3df(
                    ((int)dimensions.X * x), 
                    ((int)dimensions.Y * y), 
                    ((int)dimensions.Z * z),
                    ((int)dimensions.X * x + (int)dimensions.X), 
                    ((int)dimensions.Y * y + (int)dimensions.Y), 
                    ((int)dimensions.Z * z + (int)dimensions.Z)
                );

                if(AABBoxInFrustum(IRR.camera->getViewFrustum(), aabbox))
                {
                    if(chunkMap.find(chunkPos) != chunkMap.end()) { //Apparently checking a value is enough to generate an object
                        if(meshMap.find(chunkPos) == meshMap.end())
                        {
                            meshMap[chunkPos] = ChunkMesh();
                            meshMap[chunkPos].Initialize(this, chunkPos);
                        }

                        if(meshMap[chunkPos].Meshed)
                        {
                            Mesh->addMeshBuffer(meshMap[chunkPos].buffer);
                        }
                        if(!chunkMap[chunkPos].empty)
                        {
                            if(chunkMap[chunkPos].status == FILLED || chunkMap[chunkPos].status == DIRTY)
                            {
                                chunkMap[chunkPos].status = MESHING;
                                boost::threadpool::schedule(
                                    (*IRR.mThreadPool), 
                                    boost::threadpool::prio_task_func(10, boost::bind(&VoxelSceneNode::MeshBackground, this, chunkPos))
                                );
                            }
                        }
                        if(chunkMap[chunkPos].obstruct)
                        {
                            y = aabboxStart.Y;
                        }
                    }
                }
            }
        }
    }

    Mesh->recalculateBoundingBox();
    Mesh->setDirty();
}

void VoxelSceneNode::preRenderCalculationsIfNeeded()
{
    preRenderFillChunks();
    preRenderMeshChunks();
}

void VoxelSceneNode::FillBackground(const irr::core::vector3d<int> chunkPos)
{


    /*printf("Background Filling (%i,%i,%i) (%i,%i,%i)\n", 
        chunkPos.X, chunkPos.Y, chunkPos.Z,
        chunkMap[chunkPos].localPoint.X,
        chunkMap[chunkPos].localPoint.Y,
        chunkMap[chunkPos].localPoint.Z
        );
        */
    chunkMap[chunkPos].FillChunk(noiseTree);

    //Recursively chunk meshes, depending on depth.
    if(!chunkMap[chunkPos].obstruct)
    {
        //printf("Adding (%i,%i,%i) to queue\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
        irr::core::vector3d<int> down(chunkPos.X, chunkPos.Y - 1, chunkPos.Z);

        if(chunkMap.find(down) == chunkMap.end())
        {
            //printf("Initializing (%i,%i,%i) to queue\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
            chunkMap[down] = VoxelChunk();
            chunkMap[down].Initialize(this, down);
        }

        chunkMap[down].status = FILLING;

        boost::threadpool::schedule(
            (*IRR.mThreadPool), 
            boost::threadpool::prio_task_func(1, boost::bind(&VoxelSceneNode::FillBackground, this, down))
        );
    }
    else
    {
        //printf("This block obstructs, no point in going down\n");
    }

	dirty = true;
	chunkMap[chunkPos].status = FILLED;
}

void VoxelSceneNode::MeshBackground(const irr::core::vector3d<int> chunkPos)
{
    meshMap[chunkPos].Initialize(this, chunkPos);
	meshMap[chunkPos].GenerateMesh();

	if(chunkMap[chunkPos].status == MESHING)
		chunkMap[chunkPos].status = CLEAN;
}

//TODO: reimplement to cast a ray from postition in a direction
int VoxelSceneNode::GetAltitude(const irr::core::vector3df & Position)
{
return GetAltitude(irr::core::vector3d<int>(
    (int) Position.X,
    (int) Position.Y,
    (int) Position.Z
));
}

int VoxelSceneNode::GetAltitude(const irr::core::vector3d<int> & Position)
{
	unsigned y;

	int height;

	for(y = 0; y < 16; y++) {
	    VoxelReference vr(irr::core::vector3d<int>(Position.X, Position.Y - (y * dimensions.Y), Position.Z));

	    chunkMapIterator = chunkMap.find(vr.Chunk);

	    if(chunkMapIterator != chunkMap.end())
	    {
	        height = chunkMapIterator->second.GetHeight(irr::core::vector2d<int>(vr.Position.X, vr.Position.Z));

	        if(height >= 0)
	        {
	            return (vr.Chunk.Y * dimensions.Y) + height;
	        }
	    }
	}

	return -1;
}
void VoxelSceneNode::SetMaterial(irr::core::vector3d<int> Position, int value)
{
    VoxelReference vr(Position);

    if(chunkMap.find(vr.Chunk) != chunkMap.end())
    {
        chunkMap[vr.Chunk].SetMaterial(vr.Position, value);
    }
}
void VoxelSceneNode::SetValue(irr::core::vector3d<int> Position, float value)
{
    VoxelReference vr(Position);

    if(chunkMap.find(vr.Chunk) != chunkMap.end())
    {
        chunkMap[vr.Chunk].SetValue(vr.Position, value);
    }
}

float VoxelSceneNode::GetValue(irr::core::vector3d<int> Position)
{
    VoxelReference vr(Position);

    if(chunkMap.find(vr.Chunk) != chunkMap.end())
    {
        if(chunkMap[vr.Chunk].status != EMPTY && chunkMap[vr.Chunk].status != FILLING)
        {
            return chunkMap[vr.Chunk].GetValue(vr.Position);
        }
        else
        {
            return -15.f;
        }
    }
    else
    {
        return -10.f;
    }
}

int VoxelSceneNode::GetMaterial(irr::core::vector3d<int> Position)
{
    VoxelReference vr(Position);

    if(chunkMap.find(vr.Chunk) != chunkMap.end())
    {
        if(chunkMap[vr.Chunk].status != EMPTY && chunkMap[vr.Chunk].status != FILLING)
        {
            return chunkMap[vr.Chunk].GetMaterial(vr.Position);
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -2;
    }
}

void VoxelSceneNode::UpdateVoxel(VoxelData & vd, bool subtract)
{
    VoxelReference vr(vd.Position);

    chunkMapIterator = chunkMap.find(vr.Chunk);

    if(chunkMap.find(vr.Chunk) == chunkMap.end())
    {
        //chunkMap[vr.Chunk] = VoxelChunk();
        //chunkMap[vr.Chunk].Initialize(dimensions, vr.Chunk);
        
        //meshMap[vr.Chunk] = ChunkMesh();
        //meshMap[vr.Chunk].Initialize(this, vr.Chunk);
        
    }

    chunkMap[vr.Chunk].UpdateVoxel(vr.Position, vd.Value, vd.Material, subtract);
    chunkMap[vr.Chunk].status = DIRTY;
}

void VoxelSceneNode::AddBrush(irr::core::vector3df Position)
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

    dirty = true;
}

void VoxelSceneNode::RemoveBrush(irr::core::vector3df Position)
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

    dirty = true;
}

void VoxelSceneNode::render()
{
	irr::video::IVideoDriver * driver = SceneManager->getVideoDriver();
/*
	if(!Mesh || !driver)
		return;
*/
	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
	
	Box = Mesh->getBoundingBox();

	//printf("Rendering %i buffers\n", Mesh->getMeshBufferCount());

	for (u32 i = 0; i < Mesh->getMeshBufferCount(); ++i)
	{
		irr::scene::IMeshBuffer * mb = Mesh->getMeshBuffer(i);
		if(mb)
		{
			driver->setMaterial(Material);
			driver->drawMeshBuffer(mb);
		}
	}

	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
}

const irr::core::aabbox3df & VoxelSceneNode::getBoundingBox() const
{
	return Mesh->getBoundingBox();
}

irr::video::SMaterial& VoxelSceneNode::getMaterial(u32 i)
{
    return Material;
}

const irr::video::SMaterial & VoxelSceneNode::getMaterial(u32 i) const
{
	return Material;
}

u32 VoxelSceneNode::getMaterialCount() const
{
	return 1;
}