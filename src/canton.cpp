#include <iostream>
#include <irrlicht.h>
#include "driverChoice.h"

#include "canton.h"
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

        int index[6] = { 0, 1, 2, 3, 4, 5 }; 

        //fixme: this should be pushed to terrain.cpp
        services->setPixelShaderConstant("dirtTex",  &index[0], 1);
        services->setPixelShaderConstant("clayTex",  &index[1], 1);
        services->setPixelShaderConstant("grassTex", &index[2], 1);
        services->setPixelShaderConstant("rockTex",  &index[3], 1);
        services->setPixelShaderConstant("sandTex",  &index[4], 1);
        services->setPixelShaderConstant("voidTex",  &index[5], 1);


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
			), 32, false, false, true,
		&receiver);

	if(device == 0)
		return 1;

	video::IVideoDriver * driver = device->getVideoDriver();
	scene::ISceneManager * smgr = device->getSceneManager();
    gui::IGUIEnvironment * env = device->getGUIEnvironment();

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


    gui::IGUISkin* skin = env->getSkin();
    gui::IGUIFont* font = env->getFont("../../media/fonthaettenschweiler.bmp");
    if (font)
        skin->setFont(font);

    skin->setFont(env->getBuiltInFont(), gui::EGDF_TOOLTIP);

	psFileName = "./shaders/terrain.frag";
	vsFileName = "./shaders/terrain.vert";

	video::IGPUProgrammingServices * gpu = driver->getGPUProgrammingServices();
	s32 terrainMaterial = 0;

	if(gpu)
	{
		ShaderCallBack * shaderCallBack = new ShaderCallBack();

		terrainMaterial = gpu->addHighLevelShaderMaterialFromFiles(
			vsFileName, "vertexMain", video::EVST_VS_1_1,
			psFileName, "pixelMain", video::EPST_PS_1_1,
			shaderCallBack, video::EMT_SOLID);

		shaderCallBack->drop();
	}

	//FIXME: This code should be part of Terrain
	//MCubeMesh mesh(world.tc); old



	scene::IMeshSceneNode * meshnode = smgr->addMeshSceneNode(&world.Mesh);
	
    meshnode->setName("MeshNode");

	meshnode->setAutomaticCulling(scene::EAC_OFF);
	meshnode->setDebugDataVisible(scene::EDS_BBOX_BUFFERS);
	//meshnode->setHardwareMappingHint(scene::EHM_STREAM);

	meshnode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, settings->FirstChildElement("mesh")->BoolAttribute("cullbackface"));
	meshnode->setMaterialFlag(video::EMF_WIREFRAME, settings->FirstChildElement("mesh")->BoolAttribute("wireframe"));
	meshnode->setMaterialFlag(video::EMF_LIGHTING, true);
	meshnode->setMaterialTexture(0, driver->getTexture("./resources/dirt.jpg"));
	meshnode->setMaterialTexture(1, driver->getTexture("./resources/clay.jpg"));
	meshnode->setMaterialTexture(2, driver->getTexture("./resources/grass.jpg"));
	meshnode->setMaterialTexture(3, driver->getTexture("./resources/rock.jpg"));
	meshnode->setMaterialTexture(4, driver->getTexture("./resources/sand.jpg"));
	meshnode->setMaterialTexture(5, driver->getTexture("./resources/void.jpg"));
	meshnode->setMaterialType((video::E_MATERIAL_TYPE) terrainMaterial);

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

	core::vector3df position = core::vector3df(16.f, 16.f, 16.f);
	core::vector3df offset = core::vector3df(-8.f,8.f,-8.f);

	//scene::ICameraSceneNode *camera = smgr->addCameraSceneNode(0, position+offset, position);

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNodeFPS();
	camera->setPosition(position+offset);
	camera->setTarget(position);

	int lastFPS = -1;

	u32 then = device->getTimer()->getTime();
    
    const f32 MOVEMENT_SPEED = 10.f;

	while(device->run())
	{
		const u32 now = device->getTimer()->getTime();
		const f32 frameDeltaTime = (f32) (now - then) / 1000.f;

        world.generateMesh(camera->getPosition());
		
        //meshnode = smgr->addMeshSceneNode(&world.Mesh);

		then = now;

		if(device->isWindowActive())
		{
			core::vector3df cameraPosition = camera->getTarget();

			if(receiver.IsKeyDown(irr::KEY_KEY_W) || receiver.IsKeyDown(irr::KEY_UP))
				cameraPosition.Z += MOVEMENT_SPEED * frameDeltaTime;
			else if(receiver.IsKeyDown(irr::KEY_KEY_S) || receiver.IsKeyDown(irr::KEY_DOWN))
				cameraPosition.Z -= MOVEMENT_SPEED * frameDeltaTime;

			if(receiver.IsKeyDown(irr::KEY_KEY_A) || receiver.IsKeyDown(irr::KEY_LEFT))
				cameraPosition.X += MOVEMENT_SPEED * frameDeltaTime;
			else if(receiver.IsKeyDown(irr::KEY_KEY_D) || receiver.IsKeyDown(irr::KEY_RIGHT))
				cameraPosition.X -= MOVEMENT_SPEED * frameDeltaTime;

            if(receiver.IsKeyDown(irr::KEY_KEY_Q))
                cameraPosition.Y += MOVEMENT_SPEED * frameDeltaTime;
            else if(receiver.IsKeyDown(irr::KEY_KEY_Z))
                cameraPosition.Y -= MOVEMENT_SPEED * frameDeltaTime;

			//camera->setTarget(cameraPosition);
			//camera->setPosition(cameraPosition+offset);

			driver->beginScene(true, true, video::SColor(0,0,0,255));
	
			smgr->drawAll();

			//driver->setMaterial(terrainMaterial);
			/**
			* This is a horrible horrible hack - I don't know why my mesh does not render through
			* sceneManager, I can pick it up by hand and render it though.
			**/
			for(int i = 0; i < meshnode->getMesh()->getMeshBufferCount(); i++)
            {
            	driver->drawMeshBuffer(meshnode->getMesh()->getMeshBuffer(i));
            }

            //meshnode->render();

            env->drawAll();

			driver->endScene();

			int fps = driver->getFPS();
            int prims = driver->getPrimitiveCountDrawn();

            int maxvert = 0;
            int maxind = 0;

            core::list<scene::ISceneNode*> children = smgr->getRootSceneNode()->getChildren();

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

            fpsBox->setText(core::stringw(L"FPS:").append(core::stringw(fps)).c_str());
            bufferBox->setText(core::stringw(L"BUFFERS:").append(core::stringw(meshnode->getMesh()->getMeshBufferCount())).c_str());
            vertBox->setText(core::stringw(L"VERTEXES:").append(core::stringw(maxvert)).c_str());
            indBox->setText(core::stringw(L"INDEXES:").append(core::stringw(maxind)).c_str());
            primBox->setText(core::stringw(L"PRIMITIVES:").append(core::stringw(prims)).c_str());


			core::stringw str = L"Canton InDev [";
			str += driver->getName();
			str += "]";
			device->setWindowCaption(str.c_str());

		}
	}

	device->drop();

	return 0;
}
