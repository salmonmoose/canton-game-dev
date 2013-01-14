#include "shadercallback.h"

void ShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices * services, irr::s32 userData)
{
	irr::core::matrix4 invWorld = IRR.driver->getTransform(video::ETS_WORLD);
	invWorld.makeInverse();

	services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

	irr::core::matrix4 worldViewProj;
	worldViewProj = IRR.driver->getTransform(video::ETS_PROJECTION);
	worldViewProj*= IRR.driver->getTransform(video::ETS_VIEW);
	worldViewProj*= IRR.driver->getTransform(video::ETS_WORLD);

	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

	irr::core::matrix4 lightViewProj;
	lightViewProj = IRR.lightCamera->getProjectionMatrix();
	lightViewProj*= IRR.lightCamera->getViewMatrix();
	lightViewProj*= IRR.driver->getTransform(irr::video::ETS_WORLD);

	services->setVertexShaderConstant("mLightViewProj", lightViewProj.pointer(), 16);

    int index[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 }; 

    //fixme: this should be pushed to terrain.cpp
    services->setPixelShaderConstant("ShadowMap", &index[0], 1);
    services->setPixelShaderConstant("topTex0",   &index[1], 1);
    services->setPixelShaderConstant("sideTex0",  &index[2], 1);
    services->setPixelShaderConstant("topTex1",   &index[3], 1);
    services->setPixelShaderConstant("sideTex1",  &index[4], 1);
    services->setPixelShaderConstant("topTex2",   &index[5], 1);
    services->setPixelShaderConstant("sideTex2",  &index[6], 1);
    services->setPixelShaderConstant("topTex3",   &index[7], 1);
    services->setPixelShaderConstant("sideTex3",  &index[8], 1);

	irr::core::vector3df pos = IRR.device->getSceneManager()->getActiveCamera()->getAbsolutePosition();

    float camFarZ = 64.f;

    services->setPixelShaderConstant("camFarZ", &camFarZ, 1);

	services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);

	irr::video::SColorf col(0.0f, 1.0f, 1.0f, 0.0f);

	services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(&col), 4);

	irr::core::matrix4 world = IRR.driver->getTransform(video::ETS_WORLD);
	world = world.getTransposed();

	services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);
}