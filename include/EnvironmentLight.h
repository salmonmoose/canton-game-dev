#ifndef ENVIRONMENTLIGHT_H
#define ENVIRONMENTLIGHT_H

#include <irrlicht.h>
#include "Engine.h"

class EnvironmentLight
{
public:
	EnvironmentLight();
	~EnvironmentLight(){};

	void Initialize();
	void Update();
	void CalculatePosition();

	const irr::core::matrix4 Projection();


	irr::scene::ICameraSceneNode * lightCamera;
	irr::scene::ILightSceneNode * light;

	irr::core::vector3df lightCameraPosition;
protected:
	float timeOfDay; //0->1
	float timeOfYear; //0->1

	float declination;
	float rightAscension;

	float distance;	
};

#endif