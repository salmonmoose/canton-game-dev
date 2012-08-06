#include <iostream>
#include <irrlicht.h>
#include "driverChoice.h"

#include "canton.h"

#include "marchingcubes.cpp"
#include "terrain.cpp"

using namespace irr;
#ifdef linux

#elif _WIN32
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsustem:windows /ENTRY:mainCRTStartup")
#endif

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
	ScalarTerrain world;

	video::E_DRIVER_TYPE driverType = driverChoiceConsole();
	if(driverType == video::EDT_COUNT)
		return 1;

	MyEventReceiver receiver;
	IrrlichtDevice* device = createDevice(driverType,
		core::dimension2du(320, 240), 32, false, false, false,
		&receiver);

	if(device == 0)
		return 1;

	video::IVideoDriver *driver = device->getVideoDriver();
	scene::ISceneManager *smgr = device->getSceneManager();

	device->setWindowCaption(L"Canton Indev");

	MCubeMesh mesh(world.values, world.materials);

	mesh.init(driver);

	scene::IMeshSceneNode *meshnode = smgr->addMeshSceneNode(mesh.Mesh);
	meshnode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, true);

	scene::ILightSceneNode *node = smgr->addLightSceneNode(0,core::vector3df(0,0,0),
		video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 20.0f);

	if(node)
	{
		node->getLightData().Attenuation.set(0.f, 1.f/500.f, 0.f);
		scene::ISceneNodeAnimator* anim = smgr->createFlyCircleAnimator(core::vector3df(0,10,0),20.0f);
		if(anim)
		{
			node->addAnimator(anim);
			anim->drop();
		}
	}

	scene::ICameraSceneNode *camera = smgr->addCameraSceneNodeFPS();
	if(camera)
	{
		camera->setPosition(core::vector3df(0.f, 0.f, 0.f));
		camera->setTarget(core::vector3df(0.f, 0.f, 0.f));
		camera->setFarValue(200.0f);
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