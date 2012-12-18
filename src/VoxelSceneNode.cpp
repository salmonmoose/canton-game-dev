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
}

bool VoxelSceneNode::Initialize()
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
	irr::scene::ICameraSceneNode * camera = SceneManager->getActiveCamera();
	if (!camera)
		return;

	irr::core::aabbox3df bounding = camera->getViewFrustum()->getBoundingBox();

	int x_start = (int)floor(bounding.MinEdge.X / dimensions.X);
	int y_start = (int)floor(bounding.MinEdge.Y / dimensions.Y);
	int z_start = (int)floor(bounding.MinEdge.Z / dimensions.Z);

	int x_finish = (int)ceil(bounding.MaxEdge.X / dimensions.X);
	int y_finish = (int)ceil(bounding.MaxEdge.Y / dimensions.Y);
	int z_finish = (int)ceil(bounding.MaxEdge.Z / dimensions.Z);

	if((x_finish - x_start) * (y_finish - y_start) * (z_finish - z_start) > 10000)
	{
	    printf("too many buffers\n");
	    return;
	}

	Mesh.MeshBuffers.clear();

	irr::core::vector3d<int> chunkPos(0,0,0);

	irr::core::aabbox3df aabbox;

	for(int y = y_finish; y >= y_start; y--)
	{
		for(int z = z_finish; z >= z_start; z--)
		{
			for(int x = x_start; x <= x_finish; x++)
			{
	            aabbox = irr::core::aabbox3df(((int)dimensions.X * x), ((int)dimensions.Y * y), ((int)dimensions.Z * z),((int)dimensions.X * x + (int)dimensions.X), ((int)dimensions.Y * y + (int)dimensions.Y), ((int)dimensions.Z * z + (int)dimensions.Z));

	        	if(AABBoxInFrustum(camera->getViewFrustum(), aabbox))
	        	{
	        		if(meshMap[chunkPos].Meshed)
	        		{
	        			Mesh.addMeshBuffer(meshMap[chunkPos].buffer);
	        		}

	        		if((chunkMap[chunkPos].status == FILLED || chunkMap[chunkPos].status == DIRTY) && threads < MAXTHREADS && !chunkMap[chunkPos].empty)
	        		{
	        			chunkMap[chunkPos].status = MESHING;
	        			threads++;
	        			std::thread meshThread(&VoxelSceneNode::MeshBackground, this, chunkPos);
	        			//meshThread.detach();
	        			meshThread.join();
	        		}

	        		if(chunkMap[chunkPos].status == EMPTY && threads < MAXTHREADS)
	        		{
	        			chunkMap[chunkPos].status = FILLING;
	        			threads++;
	        			std::thread fillThread(&VoxelSceneNode::FillBackground, this, chunkPos);
	        			fillThread.detach();
	        			//fillThread.join();
	        		}
	        	}
			}
		}
	}
}

void VoxelSceneNode::FillBackground(irr::core::vector3d<int> chunkPos)
{
	chunkMap[chunkPos].FillChunk(noiseTree);

	for(int i = 0; i < 6; i++)
	{
		irr::core::vector3d<int> clean(chunkPos.X - Previous[i].X, chunkPos.Y - Previous[i].Y, chunkPos.Z - Previous[i].Z);

    if(chunkMap[clean].status == CLEAN || chunkMap[clean].status == MESHING)
    {
        chunkMap[clean].status = DIRTY;
    }
	}

	threads--;
	chunkMap[chunkPos].status = FILLED;
}

void VoxelSceneNode::MeshBackground(irr::core::vector3d<int> chunkPos)
{
	meshMap[chunkPos].Initialize(this, chunkPos);
	meshMap[chunkPos].GenerateMesh();

	if(chunkMap[chunkPos].status == MESHING)
		chunkMap[chunkPos].status = CLEAN;

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
}

void VoxelSceneNode::render()
{
	irr::video::IVideoDriver * driver = SceneManager->getVideoDriver();
/*
	if(!Mesh || !driver)
		return;
*/
	driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
	
	Box = Mesh.getBoundingBox();

	irr::video::SMaterial material;

	for (u32 i = 0; i < Mesh.getMeshBufferCount(); ++i)
	{
		irr::scene::IMeshBuffer * mb = Mesh.getMeshBuffer(i);
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
	return Mesh.getBoundingBox();
}

const irr::video::SMaterial & VoxelSceneNode::getMaterial(u32 i) const
{
	return Material;
}

u32 VoxelSceneNode::getMaterialCount() const
{
	return 1;
}