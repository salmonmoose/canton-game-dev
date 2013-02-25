#ifndef MOUSE_H
#define MOUSE_H

#include "irrlicht.h"
#include "Engine.h"

class Mouse
{
public:
	Mouse();
	~Mouse(){};

	irr::scene::IAnimatedMeshSceneNode * mainMesh;

    float PlayerHeight;

	void Update(float height);
	irr::core::vector3df IntersectPlane(irr::core::line3df myLine, float Plane);
    irr::core::vector3df getPosition();
    irr::core::line3df getRay();
protected:
    irr::core::vector3df Position;
};

#endif