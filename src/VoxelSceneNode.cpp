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
	        vsFileName, "vertexMain", video::EVST_VS_1_1,
	        psFileName, "pixelMain", video::EPST_PS_1_1,
	        shaderCallback, video::EMT_SOLID);

	    shaderCallback->drop();
	}

	Material.setFlag(irr::video::EMF_BACK_FACE_CULLING, true);
	Material.setFlag(irr::video::EMF_WIREFRAME, false);
	Material.setFlag(irr::video::EMF_LIGHTING, true);

	Material.MaterialType = (video::E_MATERIAL_TYPE) terrainMaterial;  

    setMaterialTexture(1, IRR.driver->getTexture("./resources/UV_mapper.jpg"));
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

void VoxelSceneNode::preRenderCalculationsIfNeeded()
{
	irr::scene::ICameraSceneNode * camera = (irr::scene::ICameraSceneNode *)SceneManager->getSceneNodeFromName("playercam");
	if (!camera)
		return;

	irr::core::aabbox3df bounding = camera->getViewFrustum()->getBoundingBox();

	irr::core::vector3d<int> newAabboxStart(
		(int)floor(bounding.MinEdge.X / dimensions.X),
		(int)floor(bounding.MinEdge.Y / dimensions.Y),
		(int)floor(bounding.MinEdge.Z / dimensions.Z)
	);

	irr::core::vector3d<int> newAabboxEnd(
		(int)ceil(bounding.MaxEdge.X / dimensions.X),
		(int)ceil(bounding.MaxEdge.Y / dimensions.Y),
		(int)ceil(bounding.MaxEdge.Z / dimensions.Z)
	);

	//printf("(%i,%i,%i)", newAabboxStart.X,newAabboxStart.Y,newAabboxStart.Z);
	//printf("(%i,%i,%i)", newAabboxEnd.X,newAabboxEnd.Y,newAabboxEnd.Z);

	//dirty = true;

	if((newAabboxStart == aabboxStart && newAabboxEnd == aabboxEnd) && !dirty)
	{
		if(!dirty) 
		{
			//printf("clean\n");
		} else {
			//printf("box matches\n");
		}

		//printf("aldready generated, why bother?\n");
        //FIXME: this doesn't work the world may change, without the the viewport changing
		return;
	}

	aabboxStart = newAabboxStart;
	aabboxEnd = newAabboxEnd;

	//printf("(%i,%i,%i)-(%i,%i,%i)\n", aabboxStart.X,aabboxStart.Y,aabboxStart.Z,aabboxEnd.X,aabboxEnd.Y,aabboxEnd.Z);

	if((aabboxEnd.X - aabboxStart.X) * (aabboxEnd.Y - aabboxStart.Y) * (aabboxEnd.Z - aabboxStart.Z) > 10000)
	{
	    printf("too many buffers\n");
	    return;
	}

	Mesh->MeshBuffers.clear();

	irr::core::aabbox3df aabbox;
/*
    aabboxStart.X = 0;
    aabboxEnd.X = 0;
    aabboxStart.Y = 2;
    aabboxEnd.Y = 2;
    aabboxStart.Z = 0;
    aabboxEnd.Z = 0;
*/
    irr::core::vector3d<int> chunkPos;

    dirty = false;

	for(int z = aabboxStart.Z; z <= aabboxEnd.Z; z++)
	{
		for(int x = aabboxStart.X; x <= aabboxEnd.X; x++)
		{
			for(int y = aabboxEnd.Y; y >= aabboxStart.Y; y--)
			{
				chunkPos.set(x,y,z);

                if(chunkMap.find(chunkPos) == chunkMap.end())
                {
                    chunkMap[chunkPos] = VoxelChunk();
                    chunkMap[chunkPos].Initialize(dimensions, chunkPos);
                    
                    meshMap[chunkPos] = ChunkMesh();
                    meshMap[chunkPos].Initialize(this, chunkPos);
                    
                }

	            aabbox = irr::core::aabbox3df(((int)dimensions.X * x), ((int)dimensions.Y * y), ((int)dimensions.Z * z),((int)dimensions.X * x + (int)dimensions.X), ((int)dimensions.Y * y + (int)dimensions.Y), ((int)dimensions.Z * z + (int)dimensions.Z));

	        	if(AABBoxInFrustum(camera->getViewFrustum(), aabbox))
	        	{
	        		if(meshMap[chunkPos].Meshed)
	        		{
	        			//printf("Adding Mesh buffer (%i,%i,%i)\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
	        			Mesh->addMeshBuffer(meshMap[chunkPos].buffer);
	        		}

	        		if((chunkMap[chunkPos].status == FILLED || chunkMap[chunkPos].status == DIRTY) && threads < MAXTHREADS && !chunkMap[chunkPos].empty)
	        		{
                        //printf("Meshing (%i,%i,%i)\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
	        			chunkMap[chunkPos].status = MESHING;
	        			threads++;
	        			std::thread meshThread(&VoxelSceneNode::MeshBackground, this, chunkPos);
	        			//meshThread.detach();
	        			meshThread.join();
	        		}

	        		//if(chunkMap[chunkPos].empty) { printf("empty\n"); }

	        		if(chunkMap[chunkPos].status == EMPTY && threads < MAXTHREADS)
	        		{
	        			//printf("Filling (%i,%i,%i)\n", chunkPos.X, chunkPos.Y, chunkPos.Z);
                        chunkMap[chunkPos].status = FILLING;
	        			threads++;
	        			std::thread fillThread(&VoxelSceneNode::FillBackground, this, chunkPos);
	        			fillThread.detach();
	        			//fillThread.join();
	        		}

	        		if(chunkMap[chunkPos].obstruct)
	        		{
	        			//printf("Obstructing Chunk, ending Y loop\n");
	        			y = aabboxStart.Y;
	        		}
	        	}
			}
		}
	}

    Mesh->recalculateBoundingBox();
    Mesh->setDirty();
}

void VoxelSceneNode::FillBackground(irr::core::vector3d<int> chunkPos)
{
	chunkMap[chunkPos].FillChunk(noiseTree);
/*
	for(int i = 0; i < 6; i++)
	{
		irr::core::vector3d<int> clean(chunkPos.X - Previous[i].X, chunkPos.Y - Previous[i].Y, chunkPos.Z - Previous[i].Z);

        if(chunkMap[clean].status == CLEAN || chunkMap[clean].status == MESHING)
        {
            chunkMap[clean].status = DIRTY;
        }
	}
*/

	dirty = true;
	threads--;
	chunkMap[chunkPos].status = FILLED;
}

void VoxelSceneNode::MeshBackground(irr::core::vector3d<int> chunkPos)
{
    meshMap[chunkPos].Initialize(this, chunkPos);
	meshMap[chunkPos].GenerateMesh();

	if(chunkMap[chunkPos].status == MESHING)
		chunkMap[chunkPos].status = CLEAN;

	dirty = true;
	threads--;
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
	        height = chunkMapIterator->second.GetHeight(irr::core::vector2d<unsigned>(vr.Position.X, vr.Position.Z));

	        if(height >= 0)
	        {
	            return (vr.Chunk.Y * dimensions.Y) + height;
	        }
	    }
	}

	return -1;
}

float VoxelSceneNode::GetValue(irr::core::vector3d<int> Position)
{
    VoxelReference vr(Position);

    chunkMapIterator = chunkMap.find(vr.Chunk);

    //printf("(%i,%i,%i)=(%i,%i,%i)+(%i,%i,%i)\n", Position.X, Position.Y, Position.Z, vr.Position.X, vr.Position.Y, vr.Position.Z, vr.Chunk.X, vr.Chunk.Y, vr.Chunk.Z);

    if(chunkMapIterator != chunkMap.end())
    {
        //return 1.f;
        return chunkMapIterator->second.GetValue(vr.Position);
    }
    else
    {
        return -10.f;
    }
}

int VoxelSceneNode::GetMaterial(irr::core::vector3d<int> Position)
{
    VoxelReference vr(Position);

    chunkMapIterator = chunkMap.find(vr.Chunk);

    if(chunkMapIterator != chunkMap.end())
    {
        return chunkMapIterator->second.GetMaterial(vr.Position);
    }
    else
    {
        return 0;
    }
}

void VoxelSceneNode::UpdateVoxel(VoxelData & vd, bool subtract)
{
    VoxelReference vr(vd.Position);

    chunkMapIterator = chunkMap.find(vr.Chunk);

    if(chunkMapIterator != chunkMap.end())
    {
        chunkMapIterator->second.UpdateVoxel(vr.Position, vd.Value, vd.Material, subtract);
        chunkMapIterator->second.status = DIRTY;

        if(vr.Position.X == 0)
            chunkMap[irr::core::vector3d<int>(vr.Chunk.X - 1, vr.Chunk.Y, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.X == dimensions.X -1)
            chunkMap[irr::core::vector3d<int>(vr.Chunk.X + 1, vr.Chunk.Y, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.Y == 0)
            chunkMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y - 1, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.Y == dimensions.Y -1)
            chunkMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y + 1, vr.Chunk.Z)].status = DIRTY;
        if(vr.Position.Z == 0)
            chunkMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y, vr.Chunk.Z - 1)].status = DIRTY;
        if(vr.Position.Z == dimensions.Z -1)
            chunkMap[irr::core::vector3d<int>(vr.Chunk.X, vr.Chunk.Y, vr.Chunk.Z + 1)].status = DIRTY;
    }
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