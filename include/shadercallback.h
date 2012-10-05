#include <irrlicht.h>
#include "engine.h"

class ShaderCallBack : public irr::video::IShaderConstantSetCallBack
{
public:
	virtual void OnSetConstants(irr::video::IMaterialRendererServices * services, irr::s32 userData);
};