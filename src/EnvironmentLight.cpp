#include "EnvironmentLight.h"

EnvironmentLight::EnvironmentLight()
{
	lightCamera = IRR.smgr->addCameraSceneNode();
	lightCamera->setName("EnvironmentLight");

	lightCameraPosition = irr::core::vector3df(-32.f, 32.f, 32.f);

	irr::core::matrix4 lightMatrix;

	lightMatrix.buildProjectionMatrixOrthoLH(120, 120, -128, 128);

	lightCamera->setProjectionMatrix(lightMatrix, true);

	light = IRR.smgr->addLightSceneNode();
}

void EnvironmentLight::Initialize()
{
	declination = 45.f;
	rightAscension = 45.f;
    distance = 32.f;
}

void EnvironmentLight::Update()
{
    rightAscension = IRR.device->getTimer()->getTime() / 24000.f;
    //declination = -cos(IRR.device->getTimer()->getTime() / 1000.f);
	CalculatePosition();

	lightCamera->setTarget(IRR.mPlayer->getPosition());
    
    lightCamera->setPosition(IRR.mPlayer->getPosition() + lightCameraPosition);
    
    light->setPosition(lightCamera->getPosition());
}

void EnvironmentLight::CalculatePosition()
{
	lightCameraPosition = irr::core::vector3df(
        0,
        sin(rightAscension),
		cos(rightAscension)
		) * distance;
}

const irr::core::matrix4 EnvironmentLight::Projection()
{
	return lightCamera->getProjectionMatrix() * lightCamera->getViewMatrix() * IRR.driver->getTransform(irr::video::ETS_WORLD);
}