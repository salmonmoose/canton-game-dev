#ifndef GLSL_MATERIAL
#define GLSL_MATERIAL

#include "irrlicht.h"
#include "Engine.h"
#include "ShaderCallback.h"

class GLSLMaterial
{
public:
    irr::io::path vsFileName;
    irr::io::path psFileName;

    irr::s32 MatID;

    GLSLMaterial();
    ~GLSLMaterial(){};
};


#endif