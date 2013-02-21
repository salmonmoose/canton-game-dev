#include <iostream>
#include "Engine.h"

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
    SetupSystem();
    SetupDevice();
	SetupScene();
    SetupGUI();
}

void IrrlichtEngineManager::SetupSystem()
{
    mThreadPool = new boost::threadpool::prio_pool(2);
    MobFactory::instance().Initialize();
    AbilityFactory::instance().Initialize();
}

void IrrlichtEngineManager::SetupDevice()
{
    irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_OPENGL;

    //device = createDevice(driverType, dimension2d<u32>(920, 540), 16, false, true, false, &receiver);
    device = createDevice(driverType, dimension2d<u32>(320, 180), 16, false, false, false, &receiver);
    
    if (!device)
        printf("Device failed to manifest\n");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    env = device->getGUIEnvironment();
    random = device->createDefaultRandomizer();
    gpu = driver->getGPUProgrammingServices();

    //FIXME: this should generate ALL materials, and probably drop them in an enum;
    if(IRR.gpu)
    {
        vsFileName = "./shaders/zdepth.vert.glsl";
        psFileName = "./shaders/zdepth.frag.glsl";
        
        ShaderCallback * shaderCallback = new ShaderCallback();

        ZDepthMaterialID = IRR.gpu->addHighLevelShaderMaterialFromFiles(
            vsFileName, "vertexMain", video::EVST_VS_3_0,
            psFileName, "pixelMain", video::EPST_PS_3_0,
            shaderCallback, video::EMT_SOLID);

        shaderCallback->drop();
    }

    ZDepthMaterial.setFlag(irr::video::EMF_BACK_FACE_CULLING, true);
    ZDepthMaterial.setFlag(irr::video::EMF_WIREFRAME, false);
    ZDepthMaterial.setFlag(irr::video::EMF_LIGHTING, true);

    ZDepthMaterial.MaterialType = (video::E_MATERIAL_TYPE) ZDepthMaterialID;  
}

void IrrlichtEngineManager::SetupScene()
{   
    smgr->setAmbientLight(video::SColorf(0.5f,0.5f,0.5f,0.5f));

    vMob = new std::vector<std::shared_ptr<Mob>>();
    vNewMobs = new std::vector<std::shared_ptr<Mob>>();
    vEnemy = new std::vector<std::shared_ptr<Mob>>();
    vPlayer = new std::vector<std::shared_ptr<Mob>>();
    vEnemyPewPew = new std::vector<std::shared_ptr<Mob>>();
    vPlayerPewPew = new std::vector<std::shared_ptr<Mob>>();

    mVoxelSceneNode = new VoxelSceneNode(smgr->getRootSceneNode(), smgr, 666);
    mVoxelSceneNode->Initialize();

    mEnvironmentLight = new EnvironmentLight();
    mEnvironmentLight->Initialize();

    AddMob("Player", vPlayer);

    mMob->AddAbility("FirePewPew");

    mPlayer = mMob;

    for(int i = 0; i < 30; i ++)
    {
        irr::core::vector2df offset = getRandomInRadius(20.f);

        AddMob("Enemy", vEnemy);

        mMob->AddAbility("FirePewPew");

        mMob->SetPosition(irr::core::vector3df(offset.X, mPlayer->getPosition().Y, offset.Y));
    }

    UpdateMobList();

    lightRenderTarget = driver->addRenderTargetTexture(irr::core::dimension2d<u32>(1024,1024), "rtt1");
    //lightRenderTarget = driver->addRenderTargetTexture(irr::core::dimension2d<u32>(64,64), "rtt1");

    camera = smgr->addCameraSceneNode();
    camera->setName("playercam");

    cameraOffset = irr::core::vector3df(-24.f, 16.f, 0.f);

    camera->setTarget(mPlayer->getPosition());
    camera->setPosition(mPlayer->getPosition() + cameraOffset);

    irr::core::matrix4 cameraMat;
    //cameraMat.buildProjectionMatrixOrthoLH(80, 45, -128, 128);
    cameraMat.buildProjectionMatrixOrthoLH(32,18, -128, 128);
    camera->setProjectionMatrix(cameraMat, true);
}

void IrrlichtEngineManager::AddMob(std::string ID, std::vector<std::shared_ptr<Mob>> * vType)
{
    std::shared_ptr<Mob> NewMob = MobFactory::instance().build_shared_ptr(ID);

    if(NewMob)
    {
        std::shared_ptr<Mob> Type = NewMob;
        mMob = NewMob;

        NewMob->Init();

        vNewMobs->push_back(NewMob);
        vType->push_back(Type);        
    }
    else
    {
        printf("Didn't Find %s\n", ID.c_str());
    }
}

void IrrlichtEngineManager::UpdateMobList()
{
    if(vNewMobs->size() > 0)
    {
        vMob->insert(vMob->end(), vNewMobs->begin(), vNewMobs->end());
        vNewMobs = new std::vector<std::shared_ptr<Mob>>();
    }
}

void IrrlichtEngineManager::SetupGUI()
{
    gui::IGUISkin* skin = env->getSkin();
    gui::IGUIFont* font = env->getFont("./resources/fonthaettenschweiler.bmp");
    if (font)
        skin->setFont(font);

    skin->setFont(env->getBuiltInFont(), gui::EGDF_TOOLTIP);

    //renderTargetDisplay = env->addImage(lightRenderTarget, irr::core::position2d<s32>(0,0), false);

    shipPosition = env->addStaticText(L"shipPosition", core::rect<s32>(0,0,200,16), true);
    shipPosition->setDrawBorder(false);
    
    then = device->getTimer()->getTime();
}

void IrrlichtEngineManager::Update()
{
	const u32 now = device->getTimer()->getTime();
	frameDeltaTime = (f32) (now - then) / 1000.f;

    then = now;

    CheckCollisions(vPlayerPewPew, vEnemy);
    CheckCollisions(vEnemyPewPew, vPlayer);

    for(vMobIterator = vMob->begin(); vMobIterator != vMob->end(); ++vMobIterator)
    {
        (*vMobIterator)->Update();
    }

    UpdateMobList();

    camera->setTarget(mPlayer->getPosition());
    camera->setPosition(mPlayer->getPosition() + cameraOffset);

    mEnvironmentLight->Update();
} 

void IrrlichtEngineManager::Draw()
{
    driver->beginScene(true, true, irr::video::SColor(255,100,100,140));

    //Shadow Pass
    driver->setRenderTarget(lightRenderTarget, true, true, irr::video::SColor(255,127,127,127));

    smgr->setActiveCamera(mEnvironmentLight->lightCamera);
    mEnvironmentLight->lightCamera->render();

    DrawZDepth(smgr->getRootSceneNode());

    //Render Pass
    smgr->setActiveCamera(camera);

    driver->setRenderTarget(0, true, true, 0);

    mVoxelSceneNode->setMaterialTexture(0, IRR.lightRenderTarget);
    
    smgr->drawAll();

    driver->draw3DLine(
        mPlayer->getPosition(),
        mPlayer->getPosition() + mEnvironmentLight->lightCameraPosition
        );

    env->drawAll();

    int fps = driver->getFPS();
    int prims = driver->getPrimitiveCountDrawn();

    irr::core::stringw str = L"Dark Equinox InDev [";
        str += driver->getName();
        str += "] fps:";
        str += fps;
        str += " rendered primitives: ";
        str += prims;
    device->setWindowCaption(str.c_str());

    driver->endScene();
}

void IrrlichtEngineManager::DrawZDepth(irr::scene::ISceneNode * node)
{
    irr::core::list<irr::scene::ISceneNode*> nodeList = node->getChildren();
    irr::core::list<irr::scene::ISceneNode*>::Iterator nodeList_iterator;

    SMaterial & mat = node->getMaterial(0);
    SMaterial tmp = mat;

    mat = ZDepthMaterial;

    node->render();

    mat = tmp;

    for(nodeList_iterator = nodeList.begin(); nodeList_iterator != nodeList.end(); nodeList_iterator++)
    {
        DrawZDepth(*nodeList_iterator);
    }
}

void IrrlichtEngineManager::CheckCollisions(const std::vector<std::shared_ptr<Mob>> * source, const std::vector<std::shared_ptr<Mob>> * target)
{
    std::vector<std::shared_ptr<Mob>>::const_iterator sourceIterator;

    for(sourceIterator = source->begin(); sourceIterator != source->end(); ++sourceIterator)
    {
        (*sourceIterator)->Collide(target);
    }
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

const int IrrlichtEngineManager::GetGameTick()
{
    return then;
}

irr::core::vector3df IrrlichtEngineManager::getRotatedVector(const irr::core::vector3df & Direction, const irr::core::vector3df & Rotation)
{
    irr::core::vector3df dir = Direction;
    irr::core::matrix4 Matrix;

    Matrix.setRotationDegrees(Rotation);
    Matrix.rotateVect(dir);

    return dir;
}

irr::core::vector2df IrrlichtEngineManager::getRandomInRadius(float radius)
{
    float t = 2 * irr::core::PI * IRR.random->frand();
    float u = IRR.random->frand() + IRR.random->frand();
    float r;

    r = (u > 1) ? 2 - u : u;

    return irr::core::vector2df(r * cos(t) * radius, r * sin(t) * radius);
}

irr::f32 IrrlichtEngineManager::getAngleBetween(const irr::core::vector3df& vec1, const irr::core::vector3df& vec2)
{
    float angle;

    float deltaX = vec2.X - vec1.X;
    float deltaZ = vec2.Z - vec1.Z;

    angle = atan2(deltaZ, deltaX) * 180 / irr::core::PI;

    return angle;
}