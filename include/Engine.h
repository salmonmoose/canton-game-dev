#ifndef _IRRLICHTENGINEMANAGER_H__
#define _IRRLICHTENGINEMANAGER_H__

#include <memory>
#include <irrlicht.h>
#include "EventReceiver.h"
#include "Mob.h"
#include <vector>
#include <map>
#include <string>
#include "GLSLMaterial.h"
#include "VoxelSceneNode.h"
#include "EnvironmentLight.h"
#include "FactoryMobFactory.h"
#include "FactoryAbilityFactory.h"
#include "threadpool.hpp"

#define IRR IrrlichtEngineManager::Instance()

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//Forward declaration
class GLSLMaterial; 

class Mob;

class MobFactory;

class VoxelSceneNode;

class EnvironmentLight;

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

	void SetupSystem();

	void SetupDevice();

	void SetupScene();

	void SetupGUI();

	void Shutdown();

	void StartRenderLoop();

	void EndRenderLoop();

	void Update();

    void UpdateMobList();

	void Draw();

	void DrawZDepth(irr::scene::ISceneNode * node);

	void DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value = irr::core::vector3df(10,10,10));

    void DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value, const irr::core::vector3df & Rotation);

    void DrawAABBox(const irr::core::aabbox3df & BoundingBox);

    void AddMob(std::string, std::vector<std::shared_ptr<Mob>> *);

    void CheckCollisions(const std::vector<std::shared_ptr<Mob>> *, const std::vector<std::shared_ptr<Mob>> *);

    const int GetGameTick();

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

	irr::video::ITexture * lightRenderTarget;
	irr::gui::IGUIImage * renderTargetDisplay;

	irr::scene::ICameraSceneNode * camera;
	irr::scene::ICameraSceneNode * lightCamera;

    irr::scene::ILightSceneNode * light;

    EnvironmentLight * mEnvironmentLight;

	gui::IGUIEnvironment * env;

	video::IGPUProgrammingServices * gpu;

	MobFactory * mMobFactory;

    std::vector<std::shared_ptr<Mob>> * vNewMobs;
    std::vector<std::shared_ptr<Mob>>::iterator vNewMobsIterator;

    std::vector<std::shared_ptr<Mob>> * vMob;
    std::vector<std::shared_ptr<Mob>>::iterator vMobIterator;

    std::vector<std::shared_ptr<Mob>> * vEnemy;
    std::vector<std::shared_ptr<Mob>>::iterator vEnemyIterator;

    std::vector<std::shared_ptr<Mob>> * vPlayer;
    std::vector<std::shared_ptr<Mob>>::iterator vPlayerIterator;

    std::vector<std::shared_ptr<Mob>> * vEnemyPewPew;
    std::vector<std::shared_ptr<Mob>>::iterator vEnemyPewPewIterator;

    std::vector<std::shared_ptr<Mob>> * vPlayerPewPew;
    std::vector<std::shared_ptr<Mob>>::iterator vPlayerPewPewIterator;

    std::shared_ptr<Mob> mPlayer;
    std::shared_ptr<Mob> mMob;

    std::map<std::string, GLSLMaterial> shaderMap;

    irr::io::path vsFileName;
    irr::io::path psFileName;
	irr::s32 ZDepthMaterialID;
    irr::video::SMaterial ZDepthMaterial;

	irr::f32 frameDeltaTime;
	irr::gui::IGUIStaticText * shipPosition;

	irr::core::vector3df cameraOffset;
	irr::core::vector3df lightCameraOffset;

	irr::scene::IMeshSceneNode * terrainMesh;

	boost::threadpool::prio_pool * mThreadPool;

	VoxelSceneNode * mVoxelSceneNode;

protected:
	IrrlichtEngineManager();

	void InitialiseVariables();

	irr::u32 then;
};

#endif