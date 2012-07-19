#include <irrlicht.h>
#include "driverChoice.h"

#include "marchingcubes.cpp"

using namespace irr;
#ifdef linux

#elif _WIN32
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsustem:windows /ENTRY:mainCRTStartup")
#endif

class CSampleSceneNode : public scene::ISceneNode
{
	core::aabbox3d<f32> Box;
	video::S3DVertex Vertices[4];
	video::SMaterial Material;

	public:
		CSampleSceneNode(
			scene::ISceneNode *parent, 
			scene::ISceneManager *mgr, 
			s32 id,
			int size
			) : scene::ISceneNode(parent, mgr, id)
		{
			Material.Wireframe = false;
			Material.Lighting = false;

			Vertices[0] = video::S3DVertex(0,0,size,1,1,0,
				video::SColor(255,0,255,255),0,1);
			Vertices[1] = video::S3DVertex(size,0,-size,1,0,0,
				video::SColor(255,255,0,255),1,1);
			Vertices[2] = video::S3DVertex(0,(size * 2),0,0,1,1,
				video::SColor(255,255,255,0),1,0);
			Vertices[3] = video::S3DVertex(-size,0,-size,0,0,1,
				video::SColor(255,0,255,0),0,0);

			Box.reset(Vertices[0].Pos);
			for(s32 i=1; i<4; ++i)
				Box.addInternalPoint(Vertices[i].Pos);
		}

		virtual void OnRegisterSceneNode()
		{
			if(IsVisible)
				SceneManager->registerNodeForRendering(this);

			ISceneNode::OnRegisterSceneNode();
		}

		virtual void render()
		{
			u16 indices[] = {
				0,2,3,
				2,1,3,
				1,0,3,
				2,0,1
			};

			video::IVideoDriver *driver = SceneManager->getVideoDriver();
			driver->setMaterial(Material);
			driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
			driver->drawVertexPrimitiveList(&Vertices[0], 4, &indices[0], 4, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
		}

		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{
			return Box;
		}

		virtual u32 getMaterialCount() const
		{
			return 1;
		}

		virtual video::SMaterial &getMaterial(u32 i)
		{
			return Material;
		}
};

class MyEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if(event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		return false;
	}

	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	MyEventReceiver()
	{
		for(u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}
private:
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
};

int main(int argc, char* argv[])
{
	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if(driverType == video::EDT_COUNT)
		return 1;

	MyEventReceiver receiver;
	IrrlichtDevice* device = createDevice(driverType,
		core::dimension2du(800, 600), 32, false, false, false,
		&receiver);

	if(device == 0)
		return 1;

	video::IVideoDriver *driver = device->getVideoDriver();
	scene::ISceneManager *smgr = device->getSceneManager();

	device->setWindowCaption(L"Canton Indev");

	MCubeMesh mesh;

	mesh.init(driver);

	scene::IMeshSceneNode* meshnode = smgr->addMeshSceneNode(mesh.Mesh);
	meshnode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);

	scene::ILightSceneNode *node = smgr->addLightSceneNode(0,core::vector3df(0,100,0),
		video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 500.0f);

	if(node)
	{
		node->getLightData().Attenuation.set(0.f, 1.f/500.f, 0.f);
		scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(core::vector3df(0,150,0),250.0f);
		if(anim)
		{
			node->addAnimator(anim);
			anim->drop();
		}
	}

	scene::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS();
	if(camera)
	{
		camera->setPosition(core::vector3df(-20.f, 150.f, -20.f));
		camera->setTarget(core::vector3df(0.f, 0.f, 0.f));
		camera->setFarValue(20000.0f);
	}

	while(device->run())
	{
		if(!device->isWindowActive())
		{
			device->sleep(100);
			continue;
		}

		driver->beginScene(true, true, video::SColor(0xff555555));
		smgr->drawAll();
		driver->endScene();
	}

	device->drop();

	return 0;
}
