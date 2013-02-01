#include "GLSLMaterial.h"

GLSLMaterial::GLSLMaterial()
{
    vsFileName = "./shaders/particle.vert";
    psFileName = "./shaders/particle.frag";

    if(IRR.gpu)
    {
        ShaderCallback * shaderCallback = new ShaderCallback();

        MatID = IRR.gpu->addHighLevelShaderMaterialFromFiles(
            vsFileName, "vertexMain", irr::video::EVST_VS_1_1,
            psFileName, "pixelMain", irr::video::EPST_PS_1_1,
            shaderCallback, video::EMT_TRANSPARENT_ALPHA_CHANNEL);

        shaderCallback->drop();
    }
}