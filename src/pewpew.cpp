#include "pewpew.h"

PewPew::PewPew(irr::core::vector3df pos, irr::core::vector3df rot, irr::core::vector3df vel)
{
    Position = pos;
    Rotation = rot;
    Velocity = vel;
    mainMesh = IRR.smgr->addAnimatedMeshSceneNode(IRR.smgr->getMesh("./resources/indevship.obj"));

    particles = IRR.smgr->addParticleSystemSceneNode(false);

    particles->setParent(mainMesh);

    emitter = particles->createBoxEmitter(
        irr::core::aabbox3d<f32>(-0.25,-0.25,-1,0.25,0.25,-1),
        irr::core::vector3df(0.0f,0.01f,0.0f),
        10,20,
        irr::video::SColor(0,255,255,255),
        irr::video::SColor(0,255,255,255),
        200,400);

    emitter->setMinStartSize(irr::core::dimension2d<f32>(0.5f,0.5f));
    emitter->setMaxStartSize(irr::core::dimension2d<f32>(0.5f,0.5f));

    particles->setEmitter(emitter);
    emitter->drop();

    irr::scene::IParticleAffector * paf = particles->createScaleParticleAffector(irr::core::dimension2d<f32>(3.0f,3.0f));
    particles->addAffector(paf);
    paf->drop();

    vsFileName = "./shaders/particle.vert";
    psFileName = "./shaders/particle.frag";

    if(IRR.gpu)
    {
        ShaderCallback * shaderCallback = new ShaderCallback();

        MatID = IRR.gpu->addHighLevelShaderMaterialFromFiles(
            vsFileName, "vertexMain", video::EVST_VS_1_1,
            psFileName, "pixelMain", video::EPST_PS_1_1,
            shaderCallback, video::EMT_TRANSPARENT_ALPHA_CHANNEL);

        shaderCallback->drop();
    }

    particles->setMaterialType((irr::video::E_MATERIAL_TYPE) MatID);

    particles->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, true);
    particles->setMaterialFlag(irr::video::EMF_WIREFRAME, false);
    particles->setMaterialFlag(irr::video::EMF_LIGHTING, true);

    particles->setMaterialTexture(0,IRR.driver->getTexture("./resources/smokecloud.png"));
}

void PewPew::Init()
{
    life = 100.f;
    MaxSpeed = 80.f;
    MaxStrafe = 0.f;
    MaxTurnRate = 45.f;
    Drag = 0.1f;
}

void PewPew::Update()
{
    Velocity += IRR.getRotatedVector(core::vector3df(0,0,1), Rotation) * MaxSpeed * IRR.frameDeltaTime;

    Mob::Update();
}