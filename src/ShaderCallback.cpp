#include "ShaderCallback.h"

void ShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices * services, irr::s32 userData)
{
	irr::core::matrix4 invWorld = IRR.driver->getTransform(video::ETS_WORLD);
	invWorld.makeInverse();

	services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

	irr::core::matrix4 worldViewProj;
	worldViewProj = IRR.driver->getTransform(video::ETS_PROJECTION);
	worldViewProj*= IRR.driver->getTransform(video::ETS_VIEW);
	worldViewProj*= IRR.driver->getTransform(video::ETS_WORLD);

	irr::core::matrix4 lightViewProj = IRR.mEnvironmentLight->Projection();


	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

	services->setVertexShaderConstant("mLightViewProj", lightViewProj.pointer(), 16);

    int index[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 }; 

    //fixme: this should be pushed to terrain.cpp
    services->setPixelShaderConstant("ShadowMap", &index[0], 1);
    services->setPixelShaderConstant("topTex0",   &index[1], 1);
    services->setPixelShaderConstant("sideTex0",  &index[2], 1);

    float camFarZ = 64.f;

    services->setPixelShaderConstant("camFarZ", &camFarZ, 1);

	irr::core::vector3df pos = IRR.mEnvironmentLight->light->getPosition();
	
    services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(& pos), 3);

	irr::video::SColorf col(2.0f, 1.5f, 0.5f, 0.0f);

	services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(& col), 4);

	irr::core::matrix4 world = IRR.driver->getTransform(video::ETS_WORLD);
	world = world.getTransposed();

	services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);
}