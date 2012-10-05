#include "shadercallback.h"

void ShaderCallBack::OnSetConstants(irr::video::IMaterialRendererServices * services, irr::s32 userData)
{
	irr::core::matrix4 invWorld = IRR.driver->getTransform(video::ETS_WORLD);
	invWorld.makeInverse();

	services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);

	irr::core::matrix4 worldViewProj;
	worldViewProj = IRR.driver->getTransform(video::ETS_PROJECTION);
	worldViewProj*= IRR.driver->getTransform(video::ETS_VIEW);
	worldViewProj*= IRR.driver->getTransform(video::ETS_WORLD);

    int index[6] = { 0, 1, 2, 3, 4, 5 }; 

    //fixme: this should be pushed to terrain.cpp
    services->setPixelShaderConstant("dirtTex",  &index[0], 1);
    services->setPixelShaderConstant("clayTex",  &index[1], 1);
    services->setPixelShaderConstant("grassTex", &index[2], 1);
    services->setPixelShaderConstant("rockTex",  &index[3], 1);
    services->setPixelShaderConstant("sandTex",  &index[4], 1);
    services->setPixelShaderConstant("voidTex",  &index[5], 1);

	services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);

	irr::core::vector3df pos = IRR.device->getSceneManager()->getActiveCamera()->getAbsolutePosition();

	services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);

	irr::video::SColorf col(0.0f, 1.0f, 1.0f, 0.0f);

	services->setVertexShaderConstant("mLightColor", reinterpret_cast<f32*>(&col), 4);

	irr::core::matrix4 world = IRR.driver->getTransform(video::ETS_WORLD);
	world = world.getTransposed();

	services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

	printf("Hitting the callback!\n");
}