#ifndef _IRRLICHTENGINEMANAGER_H__
#define _IRRLICHTENGINEMANAGER_H__

#include <irrlicht.h>
#include "eventhandler.h"
/**
* A handy macro that allows us to access the IrrlichtEngineManager singelton instance
*/

#define IRR IrrlichtEngineManager::Instance()

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


/**
* IrrlichtEngineManager is used to manage the Irrlicht 3D engine
*/

class IrrlichtEngineManager
{
public:
	/**
		Standard destructor
	*/
	~IrrlichtEngineManager();

	/**
		Returns a reference to the singelton object
	*/
	static IrrlichtEngineManager& Instance()
	{
		static IrrlichtEngineManager instance;
		return instance;
	}

	/**
		Initialises the resources needed for the Irrlicht3D engine
	*/
	void Startup();
	
	/**
		Cleans up the Irrlicht3D engine resources
	*/
	void Shutdown();

	/**
		Enters the render loop
	*/
	void StartRenderLoop();

	/**
		Stops the render loop
	*/
	void EndRenderLoop();

	/**
		Updates the world
	*/
	void Update();


	IrrlichtDevice * device;

	EventReceiver receiver;

	IVideoDriver * driver;

	ISceneManager * smgr;

	gui::IGUIEnvironment * env;

	video::IGPUProgrammingServices * gpu;

	irr::f32 frameDeltaTime;

protected:
	/**
		Standard constructor
	*/
	IrrlichtEngineManager();

	/**
		Sets all the member variables to NULL. This is called both by the constructor
		and the Shutdown function.
	*/

	void InitialiseVariables();

	irr::u32 then;

};
#endif