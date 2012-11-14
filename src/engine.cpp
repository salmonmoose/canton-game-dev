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
  //  printf ("Driver address: %p\n", driver);

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

void IrrlichtEngineManager::DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value)
{
    DrawAxis(Position, Value, irr::core::vector3df(0.f, 0.f, 0.f));
}

void IrrlichtEngineManager::DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value, const irr::core::vector3df & Rotation)
{
    driver->draw3DLine(Position, Position + getRotatedVector(irr::core::vector3df(Value.X,0,0), Rotation), SColor(255,255,0,0));
    driver->draw3DLine(Position, Position + getRotatedVector(irr::core::vector3df(0,Value.Y,0), Rotation), SColor(255,0,255,0));
    driver->draw3DLine(Position, Position + getRotatedVector(irr::core::vector3df(0,0,Value.Z), Rotation), SColor(255,0,0,255));
}

void IrrlichtEngineManager::DrawAABBox(const irr::core::aabbox3df & BoundingBox) {
    irr::core::vector3df points[8];
    points[0] = irr::core::vector3df(BoundingBox.MinEdge.X, BoundingBox.MinEdge.Y, BoundingBox.MinEdge.Z);
    points[1] = irr::core::vector3df(BoundingBox.MaxEdge.X, BoundingBox.MinEdge.Y, BoundingBox.MinEdge.Z);
    points[2] = irr::core::vector3df(BoundingBox.MinEdge.X, BoundingBox.MaxEdge.Y, BoundingBox.MinEdge.Z);
    points[3] = irr::core::vector3df(BoundingBox.MaxEdge.X, BoundingBox.MaxEdge.Y, BoundingBox.MinEdge.Z);
    points[4] = irr::core::vector3df(BoundingBox.MinEdge.X, BoundingBox.MinEdge.Y, BoundingBox.MaxEdge.Z);
    points[5] = irr::core::vector3df(BoundingBox.MaxEdge.X, BoundingBox.MinEdge.Y, BoundingBox.MaxEdge.Z);
    points[6] = irr::core::vector3df(BoundingBox.MinEdge.X, BoundingBox.MaxEdge.Y, BoundingBox.MaxEdge.Z);
    points[7] = irr::core::vector3df(BoundingBox.MaxEdge.X, BoundingBox.MaxEdge.Y, BoundingBox.MaxEdge.Z);

    driver->draw3DLine(points[0], points[1], SColor(255,255,255,0));
    driver->draw3DLine(points[0], points[2], SColor(255,255,255,0));
    driver->draw3DLine(points[3], points[1], SColor(255,255,255,0));
    driver->draw3DLine(points[3], points[2], SColor(255,255,255,0));

    driver->draw3DLine(points[4], points[5], SColor(255,255,255,0));
    driver->draw3DLine(points[4], points[6], SColor(255,255,255,0));
    driver->draw3DLine(points[7], points[5], SColor(255,255,255,0));
    driver->draw3DLine(points[7], points[6], SColor(255,255,255,0));

    driver->draw3DLine(points[0], points[4], SColor(255,255,255,0));
    driver->draw3DLine(points[1], points[5], SColor(255,255,255,0));
    driver->draw3DLine(points[2], points[6], SColor(255,255,255,0));
    driver->draw3DLine(points[3], points[7], SColor(255,255,255,0));
}

//irr::s32 getMaterialID(const std::string name)

irr::core::vector3df IrrlichtEngineManager::getRotatedVector(const irr::core::vector3df & Direction, const irr::core::vector3df & Rotation)
{
    irr::core::vector3df dir = Direction;
    irr::core::matrix4 Matrix;

    Matrix.setRotationDegrees(Rotation);
    Matrix.rotateVect(dir);

    return dir;
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

f32 IrrlichtEngineManager::getAngleBetween(const irr::core::vector3df& vec1, const irr::core::vector3df& vec2)
{
    float angle;

    float deltaX = vec2.X - vec1.X;
    float deltaZ = vec2.Z - vec1.Z;

    angle = atan2(deltaZ, deltaX) * 180 / irr::core::PI;

    return angle;
}

void IrrlichtEngineManager::EndRenderLoop()
{
	device->closeDevice();
}