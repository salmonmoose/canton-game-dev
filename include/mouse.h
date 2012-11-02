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
	irr::core::vector3d<f32> IntersectPlane(core::line3d<f32> myLine, float Plane);
    core::line3d<f32> getRay();
};

#endif