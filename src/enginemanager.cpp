#include <iostream>
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
    printf("Initializing Variables\n");
	device = NULL;
	driver  = NULL;
	smgr = NULL;
	env = NULL;
}

void IrrlichtEngineManager::Startup()
{
    SetupDevice();
	SetupScene();
    SetupGUI();
}

void IrrlichtEngineManager::SetupDevice()
{
    irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_OPENGL;
    //irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_BURNINGSVIDEO;

    device = createDevice(driverType, dimension2d<u32>(320, 240), 16, false, false, false, &receiver);

    if (!device)
        printf("Device failed to manifest\n");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    env = device->getGUIEnvironment();
    random = device->createDefaultRandomizer();
    gpu = driver->getGPUProgrammingServices();
}

void IrrlichtEngineManager::SetupScene()
{   
    smgr->setAmbientLight(video::SColorf(0.5f,0.5f,0.5f,0.5f));

    mScalarTerrain = new ScalarTerrain();
    mScalarTerrain->Init();

    terrainMesh = smgr->addMeshSceneNode(&mScalarTerrain->Mesh);

    mPlayer = new Player();

    mPlayer->Init();

    mPlayer->mainMesh->setParent(terrainMesh);

    camera = smgr->addCameraSceneNode();

    cameraOffset = irr::core::vector3df(24.f, 16.f, 0.f);

    camera->setTarget(mPlayer->getPosition());
    camera->setPosition(mPlayer->getPosition() + cameraOffset);

    irr::core::matrix4 mat;
    mat.buildProjectionMatrixOrthoLH(60, 45, 0, 256);

    camera->setProjectionMatrix(mat, true);
}

void IrrlichtEngineManager::SetupGUI()
{
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

    actualBuffers = env->addStaticText(L"frustumBuffers", core::rect<s32>(0,40,200,50), true);
    actualBuffers->setDrawBorder(false);
    
    then = device->getTimer()->getTime();
}

void IrrlichtEngineManager::Update()
{
	const u32 now = device->getTimer()->getTime();
	frameDeltaTime = (f32) (now - then) / 1000.f;

    then = now;

    mPlayer->Update();

    camera->setTarget(mPlayer->getPosition());
    camera->setPosition(mPlayer->getPosition() + cameraOffset);

    for(mobL_iterator = mobL.begin(); mobL_iterator != mobL.end(); ++mobL_iterator)
    {
        (*mobL_iterator)->Update();
    }

    mScalarTerrain->generateMesh(camera->getViewFrustum()); //FIXME: Perhaps this should depend on an active window.
} 

void IrrlichtEngineManager::Draw()
{
    driver->beginScene(true, true, irr::video::SColor(255,100,101,140));

    driver->setMaterial(mScalarTerrain->Material);
    driver->setTransform(irr::video::ETS_WORLD, terrainMesh->getAbsoluteTransformation());
    for(unsigned i = 0; i < terrainMesh->getMesh()->getMeshBufferCount(); i++)
    {
        driver->drawMeshBuffer(terrainMesh->getMesh()->getMeshBuffer(i));
    }
    smgr->drawAll();

    env->drawAll();

    int fps = driver->getFPS();
    int prims = driver->getPrimitiveCountDrawn();

    irr::core::stringw str = L"Canton InDev [";
        str += driver->getName();
        str += "] fps:";
        str += fps;
        str += " rendered primitives: ";
        str += prims;
    device->setWindowCaption(str.c_str());

    driver->endScene();
}

void IrrlichtEngineManager::Shutdown()
{
	device->drop();

	InitialiseVariables();
}

void IrrlichtEngineManager::EndRenderLoop()
{
	device->closeDevice();
}