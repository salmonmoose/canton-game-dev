#ifndef MOUSE_H
#define MOUSE_H

#include "irrlicht.h"
#include "engine.h"

class Mouse
{
public:
	Mouse();
	~Mouse(){};

	irr::scene::IAnimatedMeshSceneNode * mainMesh;

	void Update();
	irr::core::vector3df IntersectPlane(irr::core::line3df myLine, float Plane);
    irr::core::vector3df getPosition();
    irr::core::line3df getRay();
protected:
    irr::core::vector3df Position;
};

#endif