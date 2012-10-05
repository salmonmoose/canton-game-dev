#include <iostream>
#include <irrlicht.h>

#include "canton.h"
#include "engine.h"
#include "terrain.h"

//#include "marchingcubes.cpp"
//#include "terrain.cpp"

#ifdef linux

#elif _WIN32
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsustem:windows /ENTRY:mainCRTStartup")
#endif

IrrlichtDevice * device = 0;


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
	IRR.Startup();

	ScalarTerrain terrain;

	MyEventReceiver receiver;

	scene::IMeshSceneNode * meshnode = IRR.smgr->addMeshSceneNode(&terrain.Mesh);
	scene::IMeshSceneNode * cube = IRR.smgr->addCubeSceneNode();
	
    meshnode->setName("MeshNode");

	scene::ILightSceneNode * node = IRR.smgr->addLightSceneNode(0,core::vector3df(0,0,0),
		video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 20.0f);

	if(node)
	{
		node->getLightData().Attenuation.set(0.f, 1.f/500.f, 0.f);
		scene::ISceneNodeAnimator * anim = IRR.smgr->createFlyCircleAnimator(core::vector3df(0,10,0),20.0f);
		if(anim)
		{
			node->addAnimator(anim);
			anim->drop();
		}
	}

	core::vector3df position = core::vector3df(16.f, 16.f, 16.f);
	core::vector3df offset = core::vector3df(-8.f,8.f,-8.f);

	//scene::ICameraSceneNode *camera = smgr->addCameraSceneNode(0, position+offset, position);

	scene::ICameraSceneNode* camera = IRR.smgr->addCameraSceneNodeFPS();

	camera->setPosition(position+offset);
	camera->setTarget(position);

	int lastFPS = -1;

	u32 then = IRR.device->getTimer()->getTime();
    
    const f32 MOVEMENT_SPEED = 10.f;

	while(IRR.device->run())
	{
		const u32 now = IRR.device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32) (now - then) / 1000.f;

        terrain.generateMesh(camera->getPosition());
		
        //meshnode = smgr->addMeshSceneNode(&world.Mesh);

		then = now;

		if(IRR.device->isWindowActive())
		{
			IRR.driver->beginScene(true, true, video::SColor(0,0,0,255));
	
			IRR.smgr->drawAll();

			IRR.driver->setMaterial(terrain.Material);

			for(int i = 0; i < meshnode->getMesh()->getMeshBufferCount(); i++)
            {
            	IRR.driver->drawMeshBuffer(meshnode->getMesh()->getMeshBuffer(i));
            }

            //meshnode->render();

            IRR.env->drawAll();

			IRR.driver->endScene();

			int fps = IRR.driver->getFPS();
            int prims = IRR.driver->getPrimitiveCountDrawn();

            int maxvert = 0;
            int maxind = 0;

            core::list<scene::ISceneNode*> children = IRR.smgr->getRootSceneNode()->getChildren();

            for(int i = 0; i < meshnode->getMesh()->getMeshBufferCount(); i++)
            {
                if(meshnode->getMesh()->getMeshBuffer(i)->getVertexCount() > maxvert) 
                {
                    maxvert = meshnode->getMesh()->getMeshBuffer(i)->getVertexCount();
                }

                if(meshnode->getMesh()->getMeshBuffer(i)->getIndexCount() > maxind) 
                {
                    maxind = meshnode->getMesh()->getMeshBuffer(i)->getIndexCount();
                }
            }

			core::stringw str = L"Canton InDev [";
			str += IRR.driver->getName();
			str += "]";
			IRR.device->setWindowCaption(str.c_str());

		}
	}

	IRR.device->drop();

	return 0;
}
