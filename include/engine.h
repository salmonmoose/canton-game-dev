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

	void DrawAxis(const irr::core::vector3df & pos, const irr::core::vector3df & size = irr::core::vector3df(10,10,10));

	IrrlichtDevice * device;

	EventReceiver receiver;

	IVideoDriver * driver;

	ISceneManager * smgr;

	gui::IGUIEnvironment * env;

	video::IGPUProgrammingServices * gpu;

	irr::f32 frameDeltaTime;
	irr::gui::IGUIStaticText * fillThreads;
	irr::gui::IGUIStaticText * meshThreads;
	irr::gui::IGUIStaticText * boxBuffers;
	irr::gui::IGUIStaticText * frustumBuffers;

protected:
	IrrlichtEngineManager();

	void InitialiseVariables();

	irr::u32 then;
};
#endif