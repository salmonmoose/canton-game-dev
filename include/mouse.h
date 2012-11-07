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
	irr::core::vector3df Position;

	void Update();
	irr::core::vector3d<irr::f32> IntersectPlane(irr::core::line3d<irr::f32> myLine, float Plane);
    irr::core::line3d<irr::f32> getRay();
};

#endif