#include "engine.h"

IrrlichtEngineManager::IrrlichtEngineManager()
{
	InitialiseVariables();
}

IrrlichtEngineManager::~IrrlichtEngineManager()
{

}

void IrrlichtEngineManager::InitialiseVariables()
{
	device = NULL;
	driver  = NULL;
	smgr = NULL;
	env = NULL;
}

void IrrlichtEngineManager::Startup()
{
	irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_OPENGL;

	device = createDevice(driverType, dimension2d<u32>(640, 480));

	// Set the window title

	device->setWindowCaption(L"Irrlicht Tutorial");

	// get pointers to the video driver and the scene manager for convenience

	driver = device->getVideoDriver();

	smgr = device->getSceneManager();

	env = device->getGUIEnvironment();

	gpu = driver->getGPUProgrammingServices();

	gui::IGUISkin* skin = env->getSkin();
    gui::IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
    if (font)
        skin->setFont(font);

    skin->setFont(env->getBuiltInFont(), gui::EGDF_TOOLTIP);

    gui::IGUIStaticText * fpsBox = env->addStaticText(L"FPS", core::rect<s32>(0,0,200,10), true);
    fpsBox->setDrawBorder(false);
    gui::IGUIStaticText * primBox = env->addStaticText(L"PRIMS", core::rect<s32>(0,10,200,20), true);
    primBox->setDrawBorder(false);
    gui::IGUIStaticText * bufferBox = env->addStaticText(L"BUFFERS", core::rect<s32>(0,20,200,30), true);
    bufferBox->setDrawBorder(false);
    gui::IGUIStaticText * vertBox = env->addStaticText(L"VERTS", core::rect<s32>(0,30,200,40), true);
    vertBox->setDrawBorder(false);
    gui::IGUIStaticText * indBox = env->addStaticText(L"INDEXES", core::rect<s32>(0,40,200,50), true);
    indBox->setDrawBorder(false);
}

 

void IrrlichtEngineManager::Shutdown()

{

	// any object that was created with a function that starts with "create" needs to be

	// deleted with a call to drop

	device->drop();

	// reset our other variables for consistency

	InitialiseVariables();

}

 

void IrrlichtEngineManager::StartRenderLoop()

{

	while(device->run())

	{

		/*

			The render loop. Begin the scene, draw the various elements, and then end the scene

		*/

		driver->beginScene(true, true, SColor(127,127,127,255));

 

		smgr->drawAll();

 

		driver->endScene();

	}

}

 

void IrrlichtEngineManager::EndRenderLoop()

{

	// make device->Run() return false

	device->closeDevice();

}
