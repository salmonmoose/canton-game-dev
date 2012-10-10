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

	device = createDevice(driverType, dimension2d<u32>(640, 480), 16, false, false, false, &receiver);

	driver = device->getVideoDriver();

	smgr = device->getSceneManager();

	env = device->getGUIEnvironment();

	gpu = driver->getGPUProgrammingServices();

	gui::IGUISkin* skin = env->getSkin();
    gui::IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
    if (font)
        skin->setFont(font);

    skin->setFont(env->getBuiltInFont(), gui::EGDF_TOOLTIP);

    fillThreads = env->addStaticText(L"fillThreads", core::rect<s32>(0,0,200,10), true);
    fillThreads->setDrawBorder(false);

    meshThreads = env->addStaticText(L"meshThreads", core::rect<s32>(0,10,200,20), true);
    meshThreads->setDrawBorder(false);

    boxBuffers = env->addStaticText(L"boxBuffers", core::rect<s32>(0,20,200,30), true);
    boxBuffers->setDrawBorder(false);
    
    frustumBuffers = env->addStaticText(L"frustumBuffers", core::rect<s32>(0,30,200,40), true);
    frustumBuffers->setDrawBorder(false);
    
    then = device->getTimer()->getTime();
}

void IrrlichtEngineManager::Update()
{
	const u32 now = device->getTimer()->getTime();
	frameDeltaTime = (f32) (now - then) / 1000.f;
} 

void IrrlichtEngineManager::Shutdown()
{
	device->drop();

	InitialiseVariables();
}

void IrrlichtEngineManager::DrawAxis(const irr::core::vector3df & pos, const irr::core::vector3df & size)
{
		driver->draw3DLine(pos, pos + irr::core::vector3df(size.X,0,0), SColor(255,255,0,0));
		driver->draw3DLine(pos, pos + irr::core::vector3df(0,size.Y,0), SColor(255,0,255,0));
		driver->draw3DLine(pos, pos + irr::core::vector3df(0,0,size.Z), SColor(255,0,0,255));
}

void IrrlichtEngineManager::StartRenderLoop()

{
	while(device->run())
	{
		driver->beginScene(true, true, SColor(127,127,127,255));

		smgr->drawAll();

		driver->endScene();
	}
}

void IrrlichtEngineManager::EndRenderLoop()
{
	device->closeDevice();
}