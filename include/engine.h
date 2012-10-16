#ifndef _IRRLICHTENGINEMANAGER_H__
#define _IRRLICHTENGINEMANAGER_H__

#include <irrlicht.h>
#include "eventhandler.h"

#define IRR IrrlichtEngineManager::Instance()

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class IrrlichtEngineManager
{
public:
	~IrrlichtEngineManager();

	static IrrlichtEngineManager& Instance()
	{
		static IrrlichtEngineManager instance;
		return instance;
	}

	void Startup();
	
	void Shutdown();

	void StartRenderLoop();

	void EndRenderLoop();

	void Update();

	void DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value = irr::core::vector3df(10,10,10));

    void DrawAxis(const irr::core::vector3df & Position, const irr::core::vector3df & Value, const irr::core::vector3df & Rotation);

    void DrawAABBox(const irr::core::aabbox3df & BoundingBox);

    irr::core::vector3df getRotatedVector(const irr::core::vector3df & Direction, const irr::core::vector3df & Rotation);

	IrrlichtDevice * device;

	EventReceiver receiver;

	IVideoDriver * driver;

	ISceneManager * smgr;

	irr::video::ITexture * renderTarget;

	gui::IGUIEnvironment * env;

	video::IGPUProgrammingServices * gpu;

	irr::f32 frameDeltaTime;
	irr::gui::IGUIStaticText * fillThreads;
	irr::gui::IGUIStaticText * meshThreads;
	irr::gui::IGUIStaticText * boxBuffers;
	irr::gui::IGUIStaticText * frustumBuffers;
	irr::gui::IGUIStaticText * actualBuffers;


protected:
	IrrlichtEngineManager();

	void InitialiseVariables();

	irr::u32 then;
};
#endif