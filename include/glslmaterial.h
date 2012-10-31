#ifndef GLSL_MATERIAL
#define GLSL_MATERIAL

#include "irrlicht.h"
#include "engine.h"
#include "shadercallback.h"

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