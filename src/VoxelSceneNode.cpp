 #include <irrlight.h>

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

class VoxelSceneNode : public irr::scene::IMeshSceneNode
{
public:


 	irr::core::aabbox<f32> Box;
 	irr::video::SMaterial Material;

 	std::map<irr::core::vector3d<int>, VoxelChunk> chunkMap;
 	std::map<irr::core::vector3d<int>, VoxelChunk>::iterator chunkMap_iterator;

 	std::map<irr::core::vector3d<int>, ChunkMesh> meshMap;
 	std::map<irr::core::vector3d<int>, ChunkMesh>::iterator meshMap_iterator;

 	static const int MAXTHREADS = 10;

 	static irr::core::vector3d<int> Previous[6] = {
	    irr::core::vector3d<int>(0,0,1),
	    irr::core::vector3d<int>(0,1,0),
	    irr::core::vector3d<int>(0,1,1),
	    irr::core::vector3d<int>(1,0,0),
	    irr::core::vector3d<int>(1,0,1),
	    irr::core::vector3d<int>(1,1,1)
	};



 	VoxelSceneNode(irr::scene::ISceneNode * parent, irr::scene::ISceneManager * manager, s32 id) : irr::scene::IMeshSceneNode(parent, manager, id)
 	{
 		Material.Wireframe = false;
 		Material.Lighting = false;
 	}

 	bool VoxelSceneNode::assignScalarGrid()
 	{

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
 	}

 	virtual void OnRegisterSceneNode()
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

	    Mesh->MeshBuffers.clear();

	    irr::core::vector3d<int> tl(0,0,0);

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
                		if(meshMap[tl].Meshed)
                		{
                			Mesh.addMeshBuffer(worldMesh[tl].buffer);
                		}

                		if((chunkMap[tl].status == FILLED || chunkMap[tl].status == DIRTY) && threads < MAXTHREADS && !chunkMap[tl].empty)
                		{
                			chunkMap[tl].status = MESHING;
                			threads++
                			std::thread thread(&VoxelSceneNode::MeshBackground, this, tl);
                			//thread.detatch();
                			thread.join();
                		}

                		if(chunkMap[tl].status == EMPTY && threads < MAXTHREADS)
                		{
                			chunkMap[tl].status = FILLING;
                			threads++
                			std::thread thread(&VoxelSceneNode::FillBackground, this, tl);
                			thread.detatch();
                			//thread.join();
                		}
                	}
	    		}
	    	}
	    }
 	}

 	void VoxelSceneNode::FillBackground(irr::core::vector3d<int> tl)
 	{
 		chunkMap[tl].FillChunk(noiseTree);

 		for(int i = 0; i < 6; i++)
 		{
 			irr::core::vector3d<int> clean(tl.X - Previous[i].X, tl.Y - Previous[i].Y, tl.Z - Previous[i].Z);

	        if(worldMap[clean].status == CLEAN || worldMap[clean].status == MESHING)
	        {
	            worldMap[clean].status = DIRTY;
	        }
 		}

 		threads--;
 		chunkMap[tl].status = FILLED;
 	}

 	void VoxelSceneNode::MeshBackground(irr::core::vector3d<int> tl)
 	{
 		meshMap[tl].Initialize(this, tl);
 		meshMap[tl].GenerateMesh();

 		if(chunkMap[tl].status == MESHING)
 			chunkMap[tl].status = CLEAN;

 		threads--;
 	}

	//TODO: reimplement to cast a ray from postition in a direction
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

 	virtual void render()
 	{
 		irr::video::IVideoDriver * driver = SceneManager->getVideoDriver();

 		if(!Mesh || !driver)
 			return;

 		driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
 		
 		Box = Mesh->getBoundingBox();

 		irr::video::SMaterial material;

 		for (u32 i = 0; i < Mesh->getMeshBufferCount(); ++i)
 		{
 			irr::scene::IMeshBuffer * mb = Mesh->getMeshBuffer(i);
 			if(mb)
 			{
 				const irr::video::SMaterial & material = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];

 				irr::video::IMaterialRenderer * mrndr = driver->getMaterialRenderer(material.MaterialType);

 				bool transparent = (rnd && rnd->isTransparent());

 				if(transparent == isTransParentPass)
 				{
 					driver->setMaterial(material);
 					driver->drawMeshBuffer(mb);
 				}
 			}
 		}

 		driver->setTransform(irr::video::ETS_WORLD, AbsoluteTransformation);
 	}

 	const irr::core::aabbox<f32> & VoxelSceneNode::getBoundingBox() const
 	{
 		return Mesh ? Mesh->getBoundingBox() : Box;
 	}

 	const irr::video::SMaterial & VoxelSceneNode::getMaterial(u32 i)
 	{
 		if(i >= Materials.size())
 			return ISceneNode::getMaterial(i);

 		return Materials[i];
 	}

 	u32 VoxelSceneNode::getMaterialCount() const
 	{
 		return Materials.size();
 	}
}