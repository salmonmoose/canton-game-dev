#ifndef _IRRLICHTENGINEMANAGER_H__
#define _IRRLICHTENGINEMANAGER_H__

#include <memory>
#include <irrlicht.h>
#include "functions.h"
#include "eventhandler.h"
#include "mob.h"
#include "player.h"
#include <vector>
#include <map>
#include <string>
#include "glslmaterial.h"
#include "terrain.h"

#define IRR IrrlichtEngineManager::Instance()

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class GLSLMaterial; //Forward declaration

class Mob;

class Player;

class ScalarTerrain;

class IrrlichtEngineManager
{
public:
	~IrrlichtEngineManager();

	static IrrlichtEngineManager & Instance()
	{
		static IrrlichtEngineManager instance;
		return instance;
	}

	void Startup();

	void SetupDevice();

	void SetupScene();

	void SetupGUI();

	void Shutdown();

	void StartRenderLoop();

	void EndRenderLoop();

	void Update();

	void Draw();

	void DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value = irr::core::vector3df(10,10,10));

    void DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value, const irr::core::vector3df & Rotation);

    void DrawAABBox(const irr::core::aabbox3df & BoundingBox);

    irr::f32 GetAltitude(const irr::core::vector3df & Position);

    irr::s32 getMaterialID(const std::string name);

    irr::core::vector3df getRotatedVector(const irr::core::vector3df & Direction, const irr::core::vector3df & Rotation);
    irr::core::vector2df getRandomInRadius(float radius);
    irr::f32 getAngleBetween(const irr::core::vector3df& vec1, const irr::core::vector3df& vec2);

	IrrlichtDevice * device;

	EventReceiver receiver;

	IVideoDriver * driver;

	ISceneManager * smgr;

    IRandomizer * random;

	irr::video::ITexture * renderTarget;

	irr::scene::ICameraSceneNode * camera;

	gui::IGUIEnvironment * env;

	video::IGPUProgrammingServices * gpu;

    std::vector<std::unique_ptr<Mob>> * vMob;
    std::vector<std::unique_ptr<Mob>>::iterator vMobIterator;

    std::map<std::string, GLSLMaterial> shaderMap;

	irr::f32 frameDeltaTime;
	irr::gui::IGUIStaticText * fillThreads;
	irr::gui::IGUIStaticText * meshThreads;
	irr::gui::IGUIStaticText * boxBuffers;
	irr::gui::IGUIStaticText * frustumBuffers;
	irr::gui::IGUIStaticText * actualBuffers;

	irr::core::vector3df cameraOffset;

	irr::scene::IMeshSceneNode * terrainMesh;

	ScalarTerrain * mScalarTerrain;

	Player * mPlayer;

protected:
	IrrlichtEngineManager();

	void InitialiseVariables();

	irr::u32 then;
};

#endif