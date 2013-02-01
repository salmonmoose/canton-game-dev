#ifndef SHADERCALLBACK_H
#define SHADERCALLBACK_H

#include <irrlicht.h>
#include "Engine.h"

class ShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
	virtual void OnSetConstants(irr::video::IMaterialRendererServices * services, irr::s32 userData);
};

#endif