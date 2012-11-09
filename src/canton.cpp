#include <iostream>
#include <irrlicht.h>

#include <canton.h>
#include <engine.h>
#include <terrain.h>
#include <player.h>

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

	scene::IMeshSceneNode * meshnode = IRR.smgr->addMeshSceneNode(&terrain.Mesh); //Maybe this should be handled in terrain.cpp

	Player player;
	
	player.mainMesh->setParent(meshnode);

	scene::ILightSceneNode * node = IRR.smgr->addLightSceneNode(0,core::vector3df(0,0,0),
		video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 20.0f);

	if(node)
	{
		node->getLightData().Attenuation.set(0.f, 1.f/500.f, 0.f);
	}

	core::vector3df offset = core::vector3df(24.f,16.f,0.f);

	IRR.camera->setTarget(player.getPosition());
	IRR.camera->setPosition(player.getPosition() + offset);

    core::matrix4 mat;
    mat.buildProjectionMatrixOrthoLH(60, 45, 0, 256);
    IRR.camera->setProjectionMatrix(mat, true);

	while(IRR.device->run())
	{
		IRR.receiver.endEventProcess();
		
        IRR.Update();

        player.Update();

		IRR.camera->setTarget(player.getPosition());
        node->setPosition(player.getPosition());
		IRR.camera->setPosition(player.getPosition() + offset);

        terrain.generateMesh(IRR.camera->getViewFrustum()); //FIXME: Perhaps this should depend on an active window.

		if(IRR.device->isWindowActive())
		{

			IRR.driver->beginScene(true, true, video::SColor(0,0,0,255));

			//HACK: Something doesn't work in scenemanager, this is a workaround
            //NB: should be moved into a scenenode
			IRR.driver->setMaterial(terrain.Material);
            IRR.driver->setTransform(video::ETS_WORLD, meshnode->getAbsoluteTransformation());
			for(int i = 0; i < meshnode->getMesh()->getMeshBufferCount(); i++)
            {
            	IRR.driver->drawMeshBuffer(meshnode->getMesh()->getMeshBuffer(i));
            }
            IRR.smgr->drawAll();

            IRR.DrawAxis(player.getPosition(), irr::core::vector3df(10.f,10.f,10.f), player.getRotation());
            IRR.DrawAxis(player.getPosition(), player.getVelocity());

            IRR.env->drawAll();

			IRR.driver->endScene();

			int fps = IRR.driver->getFPS();
            int prims = IRR.driver->getPrimitiveCountDrawn();

            core::list<scene::ISceneNode*> children = IRR.smgr->getRootSceneNode()->getChildren();

			core::stringw str = L"Canton InDev [";
			str += IRR.driver->getName();
			str += "] fps:";
			str += fps;
			str += " rendered primitives: ";
			str += prims;
			IRR.device->setWindowCaption(str.c_str());

		}

		IRR.receiver.startEventProcess();
	}

	IRR.device->drop();

	return 0;
}
