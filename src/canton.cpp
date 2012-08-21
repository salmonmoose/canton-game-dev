#include <iostream>
#include <irrlicht.h>
#include "driverChoice.h"

#include "canton.h"
#include "marchingcubes.h"
#include "terrain.h"
#include "tinyxml2.h"

//#include "marchingcubes.cpp"
//#include "terrain.cpp"

using namespace irr;
#ifdef linux

#elif _WIN32
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsustem:windows /ENTRY:mainCRTStartup")
#endif

IrrlichtDevice * device = 0;
io::path vsFileName;
io::path psFileName;

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

class ShaderCallBack : public video::IShaderConstantSetCallBack
{
public:
	virtual void OnSetConstants(video::IMaterialRendererServices * services, s32 userData)
	{
		video::IVideoDriver * driver = services->getVideoDriver();

		core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
		invWorld.makeInverse();

		services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

		core::matrix4 worldViewProj;
		worldViewProj = driver->getTransform(video::ETS_PROJECTION);
		worldViewProj*= driver->getTransform(video::ETS_VIEW);
		worldViewProj*= driver->getTransform(video::ETS_WORLD);

		services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

		core::vector3df pos = device->getSceneManager()->getActiveCamera()->getAbsolutePosition();

		services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);

		video::SColorf col(0.0f, 1.0f, 1.0f, 0.0f);

		services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(&col), 4);

		core::matrix4 world = driver->getTransform(video::ETS_WORLD);
		world = world.getTransposed();

		services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);
	}
};

int main(int argc, char* argv[])
{
	tinyxml2::XMLDocument doc;

	doc.LoadFile("config.xml");

	tinyxml2::XMLElement* settings = doc.FirstChildElement("document")->FirstChildElement("settings");

	ScalarTerrain world;

	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;
	if(driverType == video::EDT_COUNT)
		return 1;

	MyEventReceiver receiver;
	device = createDevice(driverType,
		core::dimension2du(
			settings->FirstChildElement("resolution")->IntAttribute("x"), 
			settings->FirstChildElement("resolution")->IntAttribute("y")
			), 32, false, false, false,
		&receiver);

	if(device == 0)
		return 1;

	video::IVideoDriver *driver = device->getVideoDriver();
	scene::ISceneManager *smgr = device->getSceneManager();

	psFileName = "./shaders/terrain.frag";
	vsFileName = "./shaders/terrain.vert";

	video::IGPUProgrammingServices * gpu = driver->getGPUProgrammingServices();
	s32 newMaterialType1 = 0;

	if(gpu)
	{
		ShaderCallBack * shaderCallBack = new ShaderCallBack();

		newMaterialType1 = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain", video::EVST_VS_1_1,
			psFileName, "pixelMain", video::EPST_PS_1_1,
			shaderCallBack, video::EMT_SOLID);

		shaderCallBack->drop();
	}

	MCubeMesh mesh(world.tc);

	mesh.init(driver);

	scene::IMeshSceneNode *meshnode = smgr->addMeshSceneNode(mesh.Mesh);
	
	meshnode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, settings->FirstChildElement("mesh")->BoolAttribute("cullbackface"));
	meshnode->setMaterialFlag(video::EMF_WIREFRAME, settings->FirstChildElement("mesh")->BoolAttribute("wireframe"));
	meshnode->setMaterialFlag(video::EMF_LIGHTING, true);
	meshnode->setMaterialTexture(0, driver->getTexture("./resources/dirt.jpg"));
	meshnode->setMaterialTexture(1, driver->getTexture("./resources/clay.jpg"));
	meshnode->setMaterialTexture(2, driver->getTexture("./resources/grass.jpg"));
	meshnode->setMaterialTexture(3, driver->getTexture("./resources/rock.jpg"));
	meshnode->setMaterialTexture(4, driver->getTexture("./resources/sand.jpg"));
	meshnode->setMaterialTexture(5, driver->getTexture("./resources/void.jpg"));

	meshnode->setMaterialType((video::E_MATERIAL_TYPE) newMaterialType1);

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

	scene::ICameraSceneNode *camera = smgr->addCameraSceneNode(
		0, 
		core::vector3df(20.f, 0.f, 10.f),
		core::vector3df(0.f,0.f,0.f)
		);

	int lastFPS = -1;

	while(device->run())
	{
		if(device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(255,255,0,255));
			smgr->drawAll();
			driver->endScene();

			int fps = driver->getFPS();

			if(lastFPS != fps)
			{
				core::stringw str = L"Canton InDev [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;
				device->setWindowCaption(str.c_str());

				lastFPS = fps;
			}
		}
	}

	device->drop();

	return 0;
}